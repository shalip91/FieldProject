#include "library1.h"
#include "Statistics.h"


void* Init(int N){
  Statistics *DS = new Statistics(N); // Call constructor
  return (void*)DS;
}

StatusType PlantTree(void *DS, int i, int j){
  if (DS == nullptr) return INVALID_INPUT;
  return ((Statistics*)DS)->plantTree(i, j);
}

StatusType AddFruit(void *DS, int i, int j, int fruitID, int ripeRate){
  if (DS == nullptr) return INVALID_INPUT;
  return ((Statistics*)DS)->addFruit(i, j, fruitID, ripeRate);
}

StatusType PickFruit(void *DS, int fruitID){
  if (DS == nullptr) return INVALID_INPUT;
  return ((Statistics*)DS)->pickFruit(fruitID);
}

StatusType RateFruit(void *DS, int fruitID, int ripeRate){
  if (DS == nullptr) return INVALID_INPUT;
  return ((Statistics*)DS)->rateFruit(fruitID, ripeRate);
}

StatusType GetBestFruit(void *DS, int i, int j, int *fruitID){
  if (DS == nullptr) return INVALID_INPUT;
  return ((Statistics*)DS)->getBestFruit(i, j, fruitID);
}

StatusType GetAllFruitsByRate(void *DS, int i, int j, int **fruits, int *numOfFruits){
  if (DS == nullptr) return INVALID_INPUT;
  return ((Statistics*)DS)->getAllFruitsByRate(i, j, fruits, numOfFruits);
}

StatusType UpdateRottenFruits(void *DS, int rottenBase, int rottenFactor){
  if (DS == nullptr) return INVALID_INPUT;
  return ((Statistics*)DS)->updateRottenFruits(rottenBase, rottenFactor);
}

void Quit(void** DS){
  delete (Statistics*)*DS; // Call destructor
  *DS = nullptr;
}
 