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
				_net.weights[i * _net.num_neurons + j] = ((scalar)1.0) / (scalar)(_net.num_neurons);
			}
		}
	}

	for (int p = 0; p < _net.num_neurons; p++)
	{
		_cur_pattern[p] = 0.0;
	}

	return true;
}

bool CNeuronHopfieldNetwork::_allocate_smallworld_network(const int num_neurons, scalar** patterns, const int num_patterns, const int k_neurons, double probability_rewire, const bool bPreAnalysis, CWnd* pMsgInterface)
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

	int val;
	for (int i = 0; i < _s_net.num_neurons; i++)
	{
		for (int j = 0; j < _s_net.k_neurons; j++)
		{
			val = i + j + 1; // val is the target neuron to be wired.
			if (val >= _s_net.num_neurons)
			{
				val = val - _s_net.num_neurons;
			}
			_s_net.links[i][j] = val;

			CvRNG rng = cvRNG(cvGetTickCount());
			double random_p = cvRandReal(&rng);
			
			// rewire neuron node
			if (j >= (_s_net.k_neurons) *(0.5))
			{
				if (bPreAnalysis == false)
				{
					if (random_p <= _s_net.p_rewire)
					{
						int rnd_neuron = cvRandInt(&rng) % (_s_net.num_neurons);
						while (rnd_neuron == val || rnd_neuron == i)
						{
							// in case non rewire and self wire.
							rnd_neuron = cvRandInt(&rng) % (_s_net.num_neurons);
						}
						_s_net.links[i][j] = rnd_neuron;
					}
				}
				else
				{
					_s_net.links[i][j] = _random_rewire_with_p_analysis(_s_net.k_neurons, _s_net.num_neurons, i, j);
				}
			}

			// init weights
			_s_net.weights[i][j] = 0.0;
		}
	}

	return true;
}

int CNeuronHopfieldNetwork::_random_rewire_with_p_analysis(const int kNeuron, const int nNeuron, const int i, const int j)
{
	int k = kNeuron;
	int n = nNeuron;
	int iIndex = i;
	int jIndex = j;
	int pos = (int)(j - (k * 0.5));

	int low = (pos) * ((2*n)/k);
	int high = (pos+1)*((2*n)/k);

	return _verify_local_bw_characteristic(low,high);
}

int CNeuronHopfieldNetwork::_verify_local_bw_characteristic(const int iLow, const int iHigh)
{
     CArray<int,int> b_array;
	 CArray<int,int> w_array;
	 int cnt_w=0,cnt_total=0;
	 
	 for (int x = iLow; x < iHigh; x++)
	 {
		 if (_patterns[0][x] == 1)
		 {
			 w_array.Add(x);
			 cnt_w++;
		 }
		 else
		 {
			 b_array.Add(x);
		 }
		 cnt_total++;
	 }
	
	 int p_w = cnt_w / cnt_total;
	
	 Sleep(0);
	 CvRNG rng = cvRNG(cvGetTickCount());
	 double p_random = cvRandReal(&rng);
	 
	 Sleep(0);
	 rng = cvRNG(cvGetTickCount());
	 int pos_neuron = 0, rnd_neuron = inf;
	
	 if (p_random < p_w)
	 {
		 if (w_array.GetCount() > 0)
		 {
			 pos_neuron = cvRandInt(&rng) % w_array.GetCount();
			 rnd_neuron = w_array.GetAt(pos_neuron);
		 }
	 }
	 else
	 {
		 if (b_array.GetCount() > 0)
		 {
			 pos_neuron = cvRandInt(&rng) % b_array.GetCount();
			 rnd_neuron = b_array.GetAt(pos_neuron);
		 }
	 }

	 if (rnd_neuron == inf)
	 {
		 Sleep(0);
		 RNG rng = cvRNG(cvGetTickCount());
		 pos_neuron = rng.uniform(iLow,iHigh);
	 }

	 b_array.RemoveAll();
	 w_array.RemoveAll();

	 b_array.FreeExtra();
	 w_array.FreeExtra();
	 return rnd_neuron;
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

	for (int x = 0; x < _s_net.num_neurons; x++)
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
	Sleep(0);
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

	::SendMessage(_pMsgInterface->GetSafeHwnd(), WM_TRAIN_COMPLETE, (WPARAM)_net.weights, (LPARAM)(_net.num_neurons-1));
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
				::SendMessage(_pMsgInterface->GetSafeHwnd(), WM_TRAIN_PREGRESS, NULL, NULL);
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
		::SendMessage(_pMsgInterface->GetSafeHwnd(), WM_TRAIN_COMPLETE, (WPARAM)_s_net.weights, (LPARAM)(_s_net.num_neurons-1));
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
	return (sum * 1.0) / (_s_net.num_neurons * 1.0);
}

double CNeuronHopfieldNetwork::_caculate_fc_similarity()
{
	int sum = 0;
	for (int i = 0; i < _net.num_neurons; i++)
	{
		if (_patterns[0][i] == _cur_pattern[i])
		{
			sum++;
		}
	}
	return (sum * 1.0) / (_net.num_neurons * 1.0);
}

int CNeuronHopfieldNetwork::_get_links_val(const int i, const int j)
{
	return _s_net.links[i][j]; 
}

scalar CNeuronHopfieldNetwork::_get_neurons_val(const int i, const int j)
{
	int n = _s_net.links[i][j];
	return  _patterns[0][n];
}