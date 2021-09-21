#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <opencv.hpp>

using namespace cv;

#define scalar double
#define inf  -1

typedef struct hopfield_net {
	int num_neurons;
	scalar* weights;
} hopfield_net;

typedef struct smallworld_hopfield_net {
	int num_neurons;
	int k_neurons;
	double p_rewire;
	int** links;
	scalar** weights;	
} smallworld_hopfield_net;


static hopfield_net _net;
static smallworld_hopfield_net _s_net;

class CNeuronHopfieldNetwork
{
public:
	CNeuronHopfieldNetwork();
	~CNeuronHopfieldNetwork();
	
	bool _allocate_network(const int num_neurons,scalar** patterns, const int num_patterns, CWnd* pMsgInterface);
	void _free_netwrok();

	bool _allocate_smallworld_network(const int num_neurons, scalar** patterns, const int num_patterns,const int k_neurons,double probability_rewire,const bool bPreAnalysis, CWnd* pMsgInterface);
	void _free_smallworld_network();

	void _train_network(const int iWeightMethod);
	void _train_smallworld_netwrok(const int iWeightMethod);

	void _update_weight(const UINT nMethod);
	void _hebian_method();
	void _heibian_method_smallworld();
	void _wan_abdullah_method();
	void _wan_abdullah_method_smallworld();
	
	void _update_state(const UINT nMethod,const UINT nNeuron);
	void _signum(const UINT nMethod,const UINT nNeuron);
	scalar* _get_current_pattern() { return _cur_pattern; }

	double _get_sw_clustering_coefficient(){return _sw_clustering_coefficient;}
	double _get_sw_average_path_length(){return _sw_average_path_length;}

	void _caculate_sw_clustering_coefficient();
	double _find_sw_clustering_coefficient(const UINT nNeuron);
	int _find_wires_between_neigbours(CArray<int, int>* pArray, const int val);

	void _caculate_sw_average_path_length();
	int _find_length(const int iNeuron, const int jNeuron);
	bool _is_inf(int* lMatrix, const int jNeuron);
	void _add2array(CArray<int, int>* pArray, const int val);
	int _random_rewire_with_p_analysis(const int kNeuron,const int nNeuron, const int i,const int j);
	int _verify_local_bw_characteristic(const int iLow, const int iHigh);
	double _caculate_sw_similarity();

private:
	scalar**     _patterns;
	scalar*      _cur_pattern;
	int          _num_patterns;
	double       _threshold;
	bool         _continue_train;
	int          _iterations_train;
	int          _nTopology;
	int          _weight_method;
	CWnd*        _pMsgInterface;
	double       _sw_average_path_length;
	double       _sw_clustering_coefficient;
};



