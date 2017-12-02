//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"
#include <math.h>
#include <string.h>
//
// TODO:Student Information
const char *studentName = "Sai Kishan Pampana";
const char *studentID   = "A53253039";
const char *email       = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;
int theta;	  // Threshold that is used in perceptron
int score;	  // This score is used to store the value from perceptron	

uint8_t lpred, gpred;	// These values store the prediction made by the global predictor and the local predictor
uint8_t ppred;		// The prediction that is returned in the perceptron
//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

// I prefer using unsigned values as we are dealing with bits rather than numbers

uint8_t *PHT;		// Defining this way would be helping in shifting bits
uint32_t ghistoryReg;	// The global pattern will be stored here
uint32_t mask;		// We will use this to select thre required number of bits
uint32_t mask_1;	// We will use this to mask the pc value to index to local history table 
uint32_t mask_2;	// We will use this to mask the history to the required number of bits
int count = 0;		// Used for debugging
unsigned int index;	// Used to index into various entries of different tables. 

uint8_t *choice_table, *lpred_table, *gpred_table;
uint32_t *lhistory_table;
int N;			// Number of entries in the percepton table
int **perceptron_table;	// Pointer to the percepton table
//
//TODO: Add your own Branch Predictor data structures here
//


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

uint8_t gshare(uint32_t pc)
{
  	uint32_t temp = pc^ghistoryReg;
	index = temp&mask;	
	if(PHT[index] == 0 || PHT[index] == 1)
		return NOTTAKEN;
	else
		return TAKEN;
}

void update_gshare(uint32_t pc, uint8_t outcome)
{
	if(outcome == 0)
		count++;
  	if(outcome)
  	{
  	  if(PHT[index] != ST)
  	    PHT[index]++;
  	}

  	else
  	{
  	  if(PHT[index] != SN)
  	    PHT[index]--;
  	}
	
	ghistoryReg = ghistoryReg << 1;
	ghistoryReg += outcome;	

}


uint8_t pred_global(uint32_t pc)
{
	index = ghistoryReg&mask;
	if(gpred_table[index] == 0 || gpred_table[index] == 1)
		return NOTTAKEN;
	else
		return TAKEN;	

}


uint8_t pred_local(uint32_t pc)
{
	index = pc&mask_1;	
	index = lhistory_table[index]&mask_2;
	
	if(lpred_table[index] == 0 || lpred_table[index] == 1)
		return NOTTAKEN;
	else
		return TAKEN;
}


uint8_t tournament(uint32_t pc)
{
	index = ghistoryReg&mask;
	
	if(choice_table[index] == 0 || choice_table[index] == 1)
		return gpred;
	else
		return lpred;

}

void update_tournament(uint32_t pc, uint8_t outcome)
{

	// Updating the global date based on the outcome
	index = ghistoryReg&mask;
	uint32_t temp_gh = ghistoryReg;	
	if(outcome)
	{
		if(gpred_table[index] != ST)
			gpred_table[index]++;
	}

	else
	{
		if(gpred_table[index] != SN)
			gpred_table[index]--;
	}

	
	// Updating local data based on the outcome	
	index = pc&mask_1; 
	int temp = index;
	index = lhistory_table[index]&mask_2;
	lhistory_table[temp] = lhistory_table[temp] << 1;
	lhistory_table[temp] += outcome;
	
	if(outcome)
	{
		if(lpred_table[index] != ST)
			lpred_table[index]++;
	}
	else
	{
		if(lpred_table[index] != SN)
			lpred_table[index]--;	
	}

	// Updating the choice table
	index = ghistoryReg&mask;	
	if(gpred == outcome && lpred != outcome)
	{
		if(choice_table[index] != SN)
			choice_table[index]--;
	}

	if(gpred != outcome && lpred == outcome)
	{
		if(choice_table[index] != ST)
			choice_table[index]++;
	}

	// Updating the global history bits. 	
	ghistoryReg = ghistoryReg << 1;
	ghistoryReg += outcome;

}

uint8_t perceptron(uint32_t pc)
{
	index = pc%N;
	int *weights = perceptron_table[index];
	uint32_t history = ghistoryReg;
	score = weights[0];
	int bit;
	
	for(int i = 0; i < N ; i++)
	{
		bit = history%2;
		history = history >> 1;
		if(bit == 0)
			score = score - weights[i+1];
		else if(bit ==1)
			score = score + weights[i+1];
	}

	if(score < 0)
		return NOTTAKEN;
	else
		return TAKEN;


	return NOTTAKEN;
}

void update_perceptron(uint32_t pc, uint8_t outcome)
{
	uint32_t history = ghistoryReg;
	index = pc%N;
	int *weights = perceptron_table[index];
	int bit;
	if(score < 0)
		score = 0 - score;
	if((ppred != outcome) || (score < theta))
	{
		for(int i = 0; i < N; i++)
		{
			bit = history%2;
			if(bit == outcome)
				weights[i + 1]++;
			else
				weights[i+1]--;

		}

	}

}

// Initialize the predictor
//
void init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
	switch (bpType) 
	{
 		case STATIC:
 			break;	
 		case GSHARE:
			mask = pow(2, ghistoryBits) - 1;
			printf("The value of mask = %d \n", mask);
 			PHT = malloc(pow(2, ghistoryBits)*sizeof(uint8_t));
            		memset(PHT, WN,pow(2,ghistoryBits)*sizeof(uint8_t));	// Initiating as weakly not taken 
			ghistoryReg = 0;
            		break;
		case TOURNAMENT:
			mask = pow(2, ghistoryBits) - 1;	
			mask_1 = pow(2, pcIndexBits) - 1;
			mask_2 = pow(2, lhistoryBits) - 1;
			printf("The value of masks are 0 = %d, 1 = %d, 2 = %d\n", mask, mask_1, mask_2);
			ghistoryReg = 0;

			choice_table = malloc(pow(2, ghistoryBits)*sizeof(uint8_t));
            		memset(choice_table, WN,pow(2,ghistoryBits)*sizeof(uint8_t));	// Initiating as weakly not taken 

			gpred_table = malloc(pow(2, ghistoryBits)*sizeof(uint8_t));
            		memset(gpred_table, WN,pow(2,ghistoryBits)*sizeof(uint8_t));	// Initiating as weakly not taken 

			lhistory_table = malloc(pow(2, pcIndexBits)*sizeof(uint32_t));
            		memset(lhistory_table, SN,pow(2,pcIndexBits)*sizeof(uint32_t));	// Initiating as weakly not taken 

			lpred_table = malloc(pow(2, lhistoryBits)*sizeof(uint8_t));
            		memset(lpred_table, WN,pow(2,lhistoryBits)*sizeof(uint8_t));	// Initiating as weakly not taken 

			break;
    		case CUSTOM:
			ghistoryBits = 22;
			int i = 0;
			N = 10;
			theta = 1.93*ghistoryBits + 14;
			perceptron_table = (int **)malloc(N*sizeof(int*));

			for(i = 0; i < N; i++)
			{
				perceptron_table[i] = (int *)malloc((ghistoryBits+1)*sizeof(int));
				memset(perceptron_table[i], 0, ghistoryBits*sizeof(int));
			}
			break;
    		default:
    		  	break;
	}
  
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
	switch (bpType)
	{
		case STATIC:
			return NOTTAKEN;
  	  	case GSHARE:
  	  		return gshare(pc);
	  	  	break;
  	  	case TOURNAMENT:
			gpred = pred_global(pc);
			lpred = pred_local(pc);
			return tournament(pc);
  	  	case CUSTOM:
			return perceptron(pc);
  	  	default:
			break;
  	}

  // If there is not a compatable bpType then return NOTTAKEN
	return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
	switch (bpType)
  	{
		case STATIC:
			break;
		case GSHARE:
			update_gshare(pc, outcome);
			break;
		case TOURNAMENT:
			update_tournament(pc, outcome);
			break;
		case CUSTOM:
			update_perceptron(pc, outcome);
			break;
		default:
			break;  
  	}
  
}
