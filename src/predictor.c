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

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

// I prefer using unsigned values as we are dealing with bits rather than numbers

uint8_t *PHT;		// Defining this way would be helping in shifting bits
uint32_t ghistoryReg;	// The global pattern will be stored here
uint32_t mask;		// We will use this to select thre required number of bits
int count = 0;		// Used for debugging
unsigned int index;
//
//TODO: Add your own Branch Predictor data structures here
//


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

uint8_t gshare(uint32_t pc)
{
	//printf("Entered this function gshare\n");
  	uint32_t temp = pc^ghistoryReg;
	index = temp&mask;	
	if(PHT[index] == 0 || PHT[index] == 1)
		return NOTTAKEN;
	else
		return TAKEN;
}

void update_gshare(uint32_t pc, uint8_t outcome)
{
	//printf("Entered the function update_gshare\n");
	//printf("The value of pc = %d\n", pc);
	if(outcome == 0)
		count++;
	//printf("The value of count = %d\n", count);
	//printf("Flag\n");
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
	//printf("Flag 2\n");
	
	ghistoryReg = ghistoryReg << 1;
	ghistoryReg += outcome;	

}

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
	//if(bpType == GSHARE)
  	//{
	//	PHT = (int*)malloc((int)pow(2, ghistoryBits)*sizeof(int));
   	//	for(int i = 0; i < (int)pow(2, ghistoryBits); i++)         // Setting the values to weakly not taken
    	//	{
      	//		PHT[i] = 1;
    	//	}  
  	//}
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
			break;
    		case CUSTOM:
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
  	  	case CUSTOM:
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
			break;
		case CUSTOM:
			break;
		default:
			break;  
  	}
  
}
