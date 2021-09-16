#include "pch.h"
#include "CNeuronHopfieldNetwork.h"
#include "MessageHeader.h"

CNeuronHopfieldNetwork::CNeuronHopfieldNetwork()
{
	_net.num_neurons = 0;
	_net.weights = NULL;
	_num_patterns = 0;
	_threshold = 0.0;
	_iterations_train = 10;
	_continue_train = true;
	_nTopology = 0;//0 = full connected,1 =small world
	_weight_method = 0;// 0 = hebian fc, 1 = wan fc, 2 = hebian fc, 3 wan sm
}

CNeuronHopfieldNetwork::~CNeuronHopfieldNetwork()
{

}

bool CNeuronHopfieldNetwork::_allocate_network(const int num_neurons, scalar** patterns, const int num_patterns, CWnd* pMsgInterface)
{
	_nTopology = 0;
	_patterns = patterns;
	_pMsgInterface = pMsgInterface;
	_num_patterns = num_patterns;
	_net.num_neurons = num_neurons;

	int size = _net.num_neurons*_net.num_neurons;
	if ( size >= 100000000)
	{
		return false;
	}

	_cur_pattern = (scalar*)malloc(sizeof(scalar) * num_neurons);
	_net.weights = (scalar*)malloc(sizeof(scalar*)*(_net.num_neurons*_net.num_neurons));

	//forms the symmetrical weight matrix 
	for (int i = 0; i < _net.num_neurons; i++)
	{
		for (int j = 0; j < _net.num_neurons; j++)
		{
			if (i == j)
			{
				// zero on diagonal
				_net.weights[i * _net.num_neurons + j] = ((scalar)0.0);
			}
			else
			{
				// free on others
				_net.weights[i * _net.num_neurons + j] = ((scalar)1.0) / (scalar)(_net.num_neurons - 1);
			}
		}
	}

	for (int p = 0; p < _net.num_neurons; p++)
	{
		_cur_pattern[p] = 0.0;
	}

	return true;
	 
}

bool CNeuronHopfieldNetwork::_allocate_smallworld_network(const int num_neurons, scalar** patterns, const int num_patterns, const int k_neurons, double probability_rewire, const bool bMutualAnalysis, CWnd* pMsgInterface)
{
	_nTopology = 1;
	_patterns = patterns;
	_pMsgInterface = pMsgInterface;
	_num_patterns = num_patterns;
	_s_net.num_neurons = num_neurons;
	_s_net.k_neurons = k_neurons;
	_s_net.p_rewire = probability_rewire;

	_cur_pattern = (scalar*)malloc(sizeof(scalar) * num_neurons);

	_s_net.links = (int**)malloc(sizeof(int*) * num_neurons);
	for (int k = 0; k < num_neurons; k++)
	{
		_s_net.links[k] = (int *)malloc(sizeof(int) * _s_net.k_neurons);
	}

	_s_net.weights = (scalar**)malloc(sizeof(scalar*) * num_neurons);
	for (int p = 0; p < num_neurons; p++)
	{
		_s_net.weights[p] = (scalar *)malloc(sizeof(scalar) * _s_net.k_neurons);
	}

	if (bMutualAnalysis == true)
	{
		_mi.p0_arr.RemoveAll();
		_mi.p1_arr.RemoveAll();
		_mutual_analysis(_patterns, _num_patterns);
	}

	int val;
	for (int i = 0; i < _s_net.num_neurons; i++)
	{
		for (int j = 0; j < _s_net.k_neurons; j++)
		{
			val = i + j + 1;
			if (val >= _s_net.num_neurons)
			{
				val = val - _s_net.num_neurons;
			}
			_s_net.links[i][j] = val;

			CvRNG rng = cvRNG(cvGetTickCount());;
			double random_p = cvRandReal(&rng);
			
			// rewire neuron node
			if (j > _s_net.k_neurons *(0.5))
			{
				if (random_p <= _s_net.p_rewire)
				{
					if (bMutualAnalysis == false)
					{
						int rnd_neuron = cvRandInt(&rng) % _s_net.num_neurons;
						while (rnd_neuron == val || rnd_neuron == i)
						{
							// in case non rewire and self wire.
							rnd_neuron = cvRandInt(&rng) % _s_net.num_neurons;
						}
						_s_net.links[i][j] = rnd_neuron;
					}
					else
					{
						int rnd_neuron;
						if (_patterns[0][i] == 0)
						{
							rnd_neuron = cvRandInt(&rng) % _mi.p1_arr.GetCount();
							_s_net.links[i][j] = _mi.p1_arr.GetAt(rnd_neuron);
						}
						else if(_patterns[0][i] == 1)
						{
							rnd_neuron = cvRandInt(&rng) % _mi.p0_arr.GetCount();
							_s_net.links[i][j] = _mi.p0_arr.GetAt(rnd_neuron);
						}
					}
				}
			}

			// init weights
			_s_net.weights[i][j] = 0.0;
		}
	}

	return true;
}

void CNeuronHopfieldNetwork::_free_netwrok()
{
	if (_net.num_neurons == 0)
	{
		return;
	}
	
	free(_net.weights);
	free(_cur_pattern);
	_net.weights = NULL;
	_net.num_neurons = 0;

	_cur_pattern = NULL;
}

void CNeuronHopfieldNetwork::_free_smallworld_network()
{
	if (_s_net.num_neurons == 0)
	{
		return;
	}

	for (int x = 0; x < _s_net.k_neurons; x++)
	{
		free(_s_net.links[x]);
		free(_s_net.weights[x]);
	}
	free(_s_net.links);
	free(_s_net.weights);
	free(_cur_pattern);

	_s_net.links = NULL;
	_s_net.weights = NULL;
	_s_net.num_neurons = 0;
	_s_net.k_neurons = 0;
	_s_net.p_rewire = 0.0;

	_cur_pattern = NULL;
}

void CNeuronHopfieldNetwork::_update_weight(const UINT nMethod)
{
	switch (nMethod)
	{
	 case 0:
		_hebian_method();
		break;
	 case 1:
		_wan_abdullah_method();
		break;
	 case 2:
		 _heibian_method_smallworld();
		 break;
	 case 3:
		 _wan_abdullah_method_smallworld();
		 break;
	}
}

void CNeuronHopfieldNetwork::_hebian_method()
{
	scalar weight_ij;
	for (int i = 0; i < _net.num_neurons; i++)
	{
		for (int j = 0; j < _net.num_neurons; j++)
		{
			weight_ij = ((scalar)0.0);
			for (int p = 0; p < _num_patterns; p++)
			{
				if (i != j)
				{
					weight_ij += (_patterns[p][i] * 2 - 1)*(_patterns[p][j] * 2 - 1);
				}
			}
			_net.weights[i * _net.num_neurons + j] = weight_ij;
		}
	}
}

void CNeuronHopfieldNetwork::_heibian_method_smallworld()
{
	scalar weight_ij;
	int num;
	for (int i = 0; i < _s_net.num_neurons; i++)
	{
		for (int j = 0; j < _s_net.k_neurons; j++)
		{
			weight_ij = ((scalar)0.0);
			num = _s_net.links[i][j];
			if (num != i )
			{
				for (int p = 0; p < _num_patterns; p++)
				{
					weight_ij += (_patterns[p][i] * 2 - 1)*(_patterns[p][num] * 2 - 1);
				}
			}
			_s_net.weights[i][j] = weight_ij;
		}
	}
}

void CNeuronHopfieldNetwork::_wan_abdullah_method()
{
	scalar weight_ij;
	for (int i = 0; i < _net.num_neurons; i++)
	{
		for (int j = 0; j < _net.num_neurons; j++)
		{
			weight_ij = 0;
			double temp = 0.5;
			for (int p = 0; p < _num_patterns; p++)
			{
				if (i != j)
				{
					if (_patterns[p][i] ==_patterns[p][j])
					{
						weight_ij = weight_ij + temp ;
					}
					else
					{
						weight_ij = weight_ij + temp * (-1);
					}
				}
			}
			_net.weights[i * _net.num_neurons + j] = weight_ij;
		}
	}
}

void CNeuronHopfieldNetwork::_wan_abdullah_method_smallworld()
{
	scalar weight_ij;	
	int num;
	for (int i = 0; i < _s_net.num_neurons; i++)
	{
		for (int j = 0; j < _s_net.k_neurons; j++)
		{
			weight_ij = 0.0;
			double temp = 0.5;
			num = _s_net.links[i][j];
			for (int p = 0; p < _num_patterns; p++)
			{
				if (num != i)
				{
					double temp1 = _patterns[p][num];
					if (_patterns[p][num] == _patterns[p][i])
					{
						weight_ij = weight_ij + temp ;
					}
					else
					{
						weight_ij = weight_ij + temp * (-1.0);
					}
				}
			}
			_s_net.weights[i][j] = weight_ij;
		}
		
	}
}

void CNeuronHopfieldNetwork::_update_state(const UINT nMethod, const UINT nNeuron)
{
	switch (nMethod)
	{
	 case 0:
		_signum(0,nNeuron);
		break;
	 case 1:
		_signum(1,nNeuron);
	}
}

void CNeuronHopfieldNetwork::_signum(const UINT nMethod,const UINT nNeuron)
{
	scalar neuron_update = ((scalar)0.0);
	
	switch (nMethod)
	{
	  case 0: //fully connected
		for (int i = 0; i < _net.num_neurons; i++)
		{
			if (i != nNeuron)
			{
				neuron_update += (_cur_pattern[i]) * (_net.weights[nNeuron + i * _net.num_neurons]);
			}
		}
		break;
	  case 1: //small world connected
		for (int i = 0; i < _s_net.num_neurons; i++)
	    {
		   for (int j = 0; j < _s_net.k_neurons; j++)
		   {
			  if (i == nNeuron)
			  {
				  int x = _s_net.links[nNeuron][j];
				  neuron_update += (_cur_pattern[x]) * (_s_net.weights[nNeuron][j]);
			  }
			  else if (_s_net.links[i][j] == nNeuron)
			  {
				  int y = _s_net.links[i][j];
				  neuron_update += (_cur_pattern[i]) * (_s_net.weights[i][j]);
			  }
		   }
		}
		break;
	}
	_cur_pattern[nNeuron] = neuron_update < ((scalar)_threshold) ? ((scalar)0.0) : ((scalar)1.0);
	
}

void CNeuronHopfieldNetwork::_train_network(const int iWeightMethod)
{
	if (!_pMsgInterface->GetSafeHwnd())
	{
		return;
	}
	_continue_train = true;
	_weight_method = iWeightMethod;
	scalar prev_neuron_state;

	while (_continue_train)
	{
		_update_weight(_weight_method);
		
		for (int i = 0; i < _net.num_neurons; i++)
		{
			prev_neuron_state = _cur_pattern[i];
			_update_state(0, i);
			if (_cur_pattern[i] == prev_neuron_state)
			{
				_continue_train = false;
			}
			else
			{
				_continue_train = true;
			}
			::SendMessage(_pMsgInterface->GetSafeHwnd(), WM_TRAIN_PREGRESS, NULL, NULL);
		}

		if (_iterations_train == 0)
		{
			_continue_train = false;
		}

		_iterations_train--;
	}

	::SendMessage(_pMsgInterface->GetSafeHwnd(), WM_TRAIN_COMPLETE, (WPARAM)_net.weights, (LPARAM)_net.num_neurons);
}

void CNeuronHopfieldNetwork::_train_smallworld_netwrok(const int iWeightMethod)
{
	_continue_train = true;
	_weight_method = iWeightMethod + 2;
	scalar prev_neuron_state;

	while (_continue_train)
	{
		_update_weight(_weight_method);
		for (int i = 0; i < _s_net.num_neurons; i++)
		{
			prev_neuron_state = _cur_pattern[i];
			_update_state(1, i);
			if (_cur_pattern[i] == prev_neuron_state)
			{
				_continue_train = false;
			}
			else
			{
				_continue_train = true;
			}

			if (_pMsgInterface->GetSafeHwnd())
			{
				::PostMessage(_pMsgInterface->GetSafeHwnd(), WM_TRAIN_PREGRESS, NULL, NULL);
			}
	
			if (_iterations_train == 0)
			{
				_continue_train = false;
			}
			_iterations_train--;
		}
	}
	if (_pMsgInterface->GetSafeHwnd())
	{
		::PostMessage(_pMsgInterface->GetSafeHwnd(), WM_TRAIN_COMPLETE, (WPARAM)_s_net.weights, (LPARAM)_s_net.num_neurons);
	}
}

void CNeuronHopfieldNetwork::_caculate_sw_average_path_length()
{
	double LengthTotal = 0;
	for (int x = 0; x < _s_net.num_neurons; x++)
	{
	  for (int y = 0; y < _s_net.num_neurons; y++)
	  {
		  if (x > y)
		  {
			  LengthTotal = LengthTotal + (_find_length(x, y)*1.0);
		  }
	  }
	}

	_sw_average_path_length = (2.0*LengthTotal) / (_s_net.num_neurons * (_s_net.num_neurons-1));
}

int CNeuronHopfieldNetwork::_find_length(const int iNeuron, const int jNeuron)
{
	int iLength = 1;
	int* lenMatrix = (int*)malloc(sizeof(int) * _s_net.num_neurons);

	for (int a = 0; a < _s_net.num_neurons; a++)
	{
      if (a == iNeuron)
      {
	    lenMatrix[a] = 0;
      }
      else
      {
	    lenMatrix[a] = inf;
      }
	}

	CArray<int, int> arr;

	for (int k = 0; k < _s_net.k_neurons; k++)
	{
		int val = _s_net.links[iNeuron][k];
		lenMatrix[val] = iLength;
		_add2array(&arr, val);
	}

	//Breadth first search
	while (_is_inf(lenMatrix, jNeuron))
	{
		iLength++;
		CArray<int, int> tmp;
		for (int x = 0; x < arr.GetCount(); x++)
		{
			for (int y = 0; y < _s_net.k_neurons; y++)
			{
				int val = _s_net.links[arr.GetAt(x)][y];
				if (lenMatrix[val] == inf)
				{
					lenMatrix[val] = iLength;
				}
				_add2array(&tmp, val);
			}
		}
		arr.RemoveAll();
		arr.Append(tmp);
	}

	arr.RemoveAll();
	free(lenMatrix);
	return iLength;
}

bool CNeuronHopfieldNetwork::_is_inf(int* lMatrix, const int jNeuron)
{
	bool result = false;

	if (lMatrix[jNeuron] == inf)
	{
		result = true;
	}

	return result;
}

void CNeuronHopfieldNetwork::_add2array(CArray<int, int>* pArray, const int val)
{
	for (int i = 0; i < pArray->GetCount(); i++)
	{
		if (pArray->GetAt(i) == val)
		{
			return;
		}
	}
	pArray->Add(val);
}

void CNeuronHopfieldNetwork::_caculate_sw_clustering_coefficient()
{
	double Ck = 0.0;
	for (int i = 0; i < _s_net.num_neurons; i++)
	{
		Ck = Ck + _find_sw_clustering_coefficient(i);
	}
	_sw_clustering_coefficient = Ck / _s_net.num_neurons;
}

double CNeuronHopfieldNetwork::_find_sw_clustering_coefficient(const UINT nNeuron)
{
	double Ck = 0;
	int num = nNeuron;
	int iNumOfWiresBetweenNeigbours = 0;
	CArray<int, int> arr;

	//find all neigbours of nNeuron
	for (int i = 0; i < _s_net.num_neurons; i++)
	{
		for (int j = 0; j < _s_net.k_neurons; j++)
		{
			if (i == num)
			{
				_add2array(&arr, _s_net.links[num][j]);
			}
			else if(_s_net.links[i][j] == num)
			{
				_add2array(&arr, i);
			}
		}
	}

	int n = (int)arr.GetCount();
	//find actual wires between neigbours.
	while(arr.GetCount()>0)
	{
		iNumOfWiresBetweenNeigbours += _find_wires_between_neigbours(&arr, arr.GetAt(0));
		arr.RemoveAt(0);
	}

	Ck = (2.0 * iNumOfWiresBetweenNeigbours) / (n * (n - 1));

	return Ck;
}

int CNeuronHopfieldNetwork::_find_wires_between_neigbours(CArray<int, int>* pArray, const int val)
{
	int wires = 0;

	for (int i = 0; i < pArray->GetCount(); i++)
	{
		if (val != pArray->GetAt(i))
		{
			int wires_val = 0;
			for (int j = 0; j < _s_net.k_neurons; j++)
			{
				// check if i of arr is listed in val's neigbours.
				if (_s_net.links[val][j] == pArray->GetAt(i))
				{
					wires_val = 1;
					break;
				}
			}

			if (wires_val == 0)
			{
				// check if val is listed in i's neigbours.
				for (int k = 0; k < _s_net.k_neurons; k++)
				{
					if (_s_net.links[i][k] == val)
					{
						wires_val = 1;
						break;
					}
				}
			}
			wires = wires + wires_val;
		}
	}

	return wires;
}

void CNeuronHopfieldNetwork::_mutual_analysis(scalar** patterns, const int num_patterns)
{
	int x = 0;
	int cnt_0 = 0, cnt_1 =0;
	for (int y = 0; y < _s_net.num_neurons; y++)
	{
		if (patterns[x][y] == 0)
		{
			_mi.p0_arr.Add(y);
		}
		else if (patterns[x][y] == 1)
		{
			_mi.p1_arr.Add(y);
		}
	}
}

double CNeuronHopfieldNetwork::_caculate_sw_similarity()
{
	int sum = 0;
	for (int i = 0; i < _s_net.num_neurons; i++)
	{
		if (_patterns[0][i] == _cur_pattern[i])
		{
			sum++;
		}
	}

	return (sum *1.0) / (_s_net.num_neurons *1.0);
}


