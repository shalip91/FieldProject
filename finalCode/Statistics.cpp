#include "Statistics.h"
#include <iostream>
#include <list>
#include <algorithm>
#include <map>
#include <string>

using std::string;
using std::cout;
using std::endl;
using std::lower_bound;
using std::make_pair;
using std::ostream;
using std::list;
using std::pair;
using std::map;

/***************************/
/* Fruit implementation  */
/***************************/

Fruit::Fruit(int i, int j, int id, int rate) : 
     i_(i), j_(j), fruit_ID_(id), rape_rate_(rate) {}
void  Fruit::setID(int id) {fruit_ID_ = id;}
void  Fruit::setRate(int rate) {rape_rate_ = rate;}
int  Fruit::getID() {return fruit_ID_;}
int  Fruit::getRate() {return rape_rate_;}
int  Fruit::getRow() {return i_;}
int  Fruit::getCol() {return j_;}

bool Fruit::operator == (const Fruit& other) const {
  return fruit_ID_ == other.fruit_ID_;
}
bool Fruit::operator != (const Fruit& other) const {
  return fruit_ID_ != other.fruit_ID_;
}
bool Fruit::operator < (const Fruit& other) const { 
  if (rape_rate_ == other.rape_rate_)
  	return fruit_ID_ < other.fruit_ID_;
  return rape_rate_ < other.rape_rate_;
}
bool Fruit::operator > (const Fruit& other) const { 
  if (rape_rate_ == other.rape_rate_)
  	return fruit_ID_ > other.fruit_ID_;
  return rape_rate_ > other.rape_rate_;
}

ostream& operator << (ostream& os, const Fruit& f){
  os << "{idx: (" << f.i_ << ", " << f.j_ << ")" <<
        " id: " << f.fruit_ID_ << 
        ", Rate: " << f.rape_rate_ << "}";
  return os;
}




/*******************************/
/* TreeFruitsit implementation */
/*******************************/

TreeFruits::TreeFruits(int N, int i, int j) : 
     N_(N), i_(i), j_(j), num_of_fruit_(0) {}
TreeFruits::~TreeFruits(){
  for (auto it = fruits_.begin(); it != fruits_.end(); ++it){
		Fruit& p = *it;
		delete &p;
  }
}
list<Fruit>* TreeFruits::getFruitsList() {return &fruits_;}
Fruit* TreeFruits::getBestFruit() {return &(fruits_.front());}
int TreeFruits::getNumOfFruits() {return num_of_fruit_;}
void TreeFruits::increaseNumOfFruits() { num_of_fruit_++;}
void TreeFruits::decreaseNumOfFruits() {num_of_fruit_--;}






/***********************/
/* Tree implementation */
/***********************/

Tree::Tree(int N, int i, int j, TreeFruits* ptr) : 
     N_(N), i_(i), j_(j), tree_fruits_(ptr) {}

Tree::~Tree(){ delete tree_fruits_; }
TreeFruits* 
Tree::getTreeFruitsPtr() {return tree_fruits_;}
void 
Tree::setTreeFruitsPtr(TreeFruits* ptr){ tree_fruits_ = ptr;}






/*****************************/
/* Statistics implementation */
/*****************************/

Statistics::Statistics(int N) : N_(N) {}

Statistics::~Statistics(){
  /* delete trees_ */
  for (auto& pair : trees_){
    pair.second->~Tree();
    pair.second = nullptr;
  }
  /* delete all_trees_fruit_list_ */
  for (auto& pair : all_trees_fruit_list_)
    pair.second = nullptr;
}

map<int,Tree*> 
Statistics::getTrees(){return trees_;}

map<int,TreeFruits*> 
Statistics::getFruits(){return all_trees_fruit_list_;}

StatusType 
Statistics::plantTree(int i, int j){

  /*invalid indexs of tree*/
  if (!isValidIdxs(i, j)) return INVALID_INPUT;

  /*allocate new Tree*/
  Tree* new_tree = nullptr;
  try{ new_tree = new Tree(N_, i, j); }
  catch(std::bad_alloc&){ return ALLOCATION_ERROR; }

  /* if tree found - return FALIURE */
  if (trees_.find(i*N_+j) != trees_.end()){
		delete new_tree;
		return FAILURE;
  }

  /*if tree NOT found - insert the new_tree */
  trees_[i*N_+j] = new_tree;
  
  return SUCCESS;
}

StatusType 
Statistics::addFruit(int i, int j, int fruitID, int ripeRate){
      
	// invalid input
	if (!isValidIdxs(i, j) || ripeRate <= 0 || fruitID <= 0) return INVALID_INPUT;

	/* if tree not found return FALUIRE */
	auto key_tree_pair = trees_.find(i*N_+j);
	if (key_tree_pair == trees_.end()) 
		return FAILURE;
	
	/* creat new fruit. if if faild return ALLOCATION_ERROR */
	Fruit* new_fruit = nullptr;
	try{ new_fruit = new Fruit(i, j, fruitID, ripeRate);}
	catch(std::bad_alloc&){ return ALLOCATION_ERROR; }
	
	/* if fruit is already exist - return Faliure */
	for (const auto& tree_fruits : all_trees_fruit_list_)
		for (const auto& fruit : *(tree_fruits.second->getFruitsList()))
			if (fruit == *new_fruit){
				delete new_fruit;
				return FAILURE;
			}
	
	/* get the ptr to the TreeFruit object */
	TreeFruits* tree_fruits = key_tree_pair->second->getTreeFruitsPtr();
	
	/* create new TreeFruit object. (in case needed) */
	TreeFruits* new_tree_fruits = nullptr;
	try{ new_tree_fruits = new TreeFruits(N_, i, j);}
	catch(std::bad_alloc&){ return ALLOCATION_ERROR; }

	/* case empty tree - 1. insert new fruit to it.*/
	/*                   2. increase fruit number */
	/*                   3. update the pointer to new_TreeFruit in the Tree. */

	if (tree_fruits == nullptr){
		list<Fruit>* list = new_tree_fruits->getFruitsList();
		list->push_front(*new_fruit);
		new_tree_fruits->increaseNumOfFruits();
		all_trees_fruit_list_[i*N_+j] = new_tree_fruits;
		key_tree_pair->second->setTreeFruitsPtr(new_tree_fruits);
	}

	/* case tree not empty - 1. delete the new TreeFruits object */
	/*                       2. insert the new fruit into the sorted location */
	/*                       3. increase fruit number */
	else if (tree_fruits != nullptr){
		delete new_tree_fruits;
		list<Fruit>* list = tree_fruits->getFruitsList();
		auto insert_pos = lower_bound(list->begin(), list->end(), *new_fruit);
		list->insert(insert_pos, *new_fruit);
		tree_fruits->increaseNumOfFruits();
		if (tree_fruits->getNumOfFruits() == 1)
			all_trees_fruit_list_[i*N_+j] = tree_fruits;
	}
	return SUCCESS;
} 

StatusType 
Statistics::pickFruit(int fruitID){
      
	/* invalid input */
	if (fruitID <= 0) return INVALID_INPUT;

	/* finding the fruit */
	for (auto& key_treeFruit_pair :all_trees_fruit_list_){
		list<Fruit>* fruit_list = key_treeFruit_pair.second->getFruitsList();
		auto it = find_if(fruit_list->begin(), fruit_list->end(),
											[&fruitID](Fruit& f){ return f.getID() == fruitID;});
		// fruit found - remove
		if (it != fruit_list->end()){
			fruit_list->erase(it);
			key_treeFruit_pair.second->decreaseNumOfFruits(); 

			// in case it was the last fruit
			if (key_treeFruit_pair.second->getNumOfFruits() == 0){
				key_treeFruit_pair.second = nullptr;
				all_trees_fruit_list_.erase(key_treeFruit_pair.first);
			}
			return SUCCESS;  
		}
	}

	// fruit not found - return FAILURE
	return FAILURE;
}

StatusType 
Statistics::rateFruit(int fruitID, int ripeRate){

	/* invalid input */
	if (fruitID <= 0 || ripeRate <= 0) return INVALID_INPUT;

	/* finding the fruit, creat updated fruit, removing old, inserting back*/
	for (auto& key_treeFruit_pair :all_trees_fruit_list_){
		list<Fruit>* list = key_treeFruit_pair.second->getFruitsList();
		auto it = find_if(list->begin(), list->end(),
											[&fruitID](Fruit& f){ return f.getID() == fruitID;});

		if (it != list->end()){
			// create updated copy of the fruit
			Fruit* updated_fruit = new Fruit(it->getRow(), it->getCol(),
																		it->getID(), ripeRate);
			// fruit found - remove
			list->erase(it);

			// fruit insertion
			auto insert_pos = lower_bound(list->begin(), list->end(), *updated_fruit);
			list->insert(insert_pos, *updated_fruit);

			return SUCCESS;
		}
	}

	// fruit not found - return FAILURE
	return FAILURE;
}

StatusType 
Statistics::getBestFruit(int i, int j, int* fruitID){

	/* invalid input */
	if (!isValidIdxs(i, j) || fruitID == nullptr) return INVALID_INPUT;

	/* tree not found - return failure */
	auto key_tree_pair_it = trees_.find(i*N_+j);
	if (key_tree_pair_it == trees_.end()) return FAILURE;

	/* the tree is empty */
	TreeFruits* tree_fruits = key_tree_pair_it->second->getTreeFruitsPtr();
	if (tree_fruits == nullptr || tree_fruits->getNumOfFruits() == 0){
		*fruitID = -1;
		return SUCCESS;
	}
	*fruitID = tree_fruits->getBestFruit()->getID();
	return SUCCESS;
}

StatusType  
Statistics::getAllFruitsByRate(int i, int j, int **fruits, int *numOfFruits){

	/* invalid input */
	if (!isValidIdxs(i, j) || 
			numOfFruits == nullptr|| 
			fruits == nullptr) return INVALID_INPUT;
	
	/* if tree not found return FALUIRE */
	auto key_tree_pair = trees_.find(i*N_+j);
	if (key_tree_pair == trees_.end()) 
		return FAILURE;

	/* get the ptr to the TreeFruit object */
	TreeFruits* tree_fruits = key_tree_pair->second->getTreeFruitsPtr();

	/* no fruits inside the tree */
	if ((tree_fruits != nullptr && tree_fruits->getNumOfFruits() == 0) || 
				tree_fruits == nullptr){

		*fruits = nullptr;
		*numOfFruits = 0;
		return SUCCESS;
	}

	/* there are fruits inside the tree */
	*numOfFruits = tree_fruits->getNumOfFruits();
	try{ *fruits = new int[*numOfFruits]; }
	catch(std::bad_alloc&){ return ALLOCATION_ERROR; }

	/* copy the sorted list into the array */
	int ii = 0;
	for (auto& fruit : *(tree_fruits->getFruitsList()))
		(*fruits)[ii++] = fruit.getID();
	
	
	return SUCCESS;
}

StatusType  
Statistics::updateRottenFruits(int rottenBase, int rottenFactor){
		/* invalid input */
	if (rottenBase < 1 || rottenFactor < 1) 
		return INVALID_INPUT;
	// if factor == 1 nothing change.
	if (rottenFactor == 1) return SUCCESS;
	
	for (auto key_treeFruits_pair : all_trees_fruit_list_){
		list<Fruit>* main_list = key_treeFruits_pair.second->getFruitsList();
		list<Fruit> tmp_list(*main_list);

		/* update all rotten fruits and erase the rest */
		for (auto it = main_list->begin(); it != main_list->end(); ){
			int id = it->getID();
			int rate = it->getRate();
			if (id % rottenBase == 0){
				it->setRate(rate * rottenFactor);
				rate = it->getRate();
				++it;
			}
			else
				it = main_list->erase(it);
		}

		// /* erase all the rotten fruits */
		for (auto it = tmp_list.begin(); it != tmp_list.end(); ){
			int id = it->getID();
			int rate = it->getRate();
			if (it->getID() % rottenBase == 0){
				it = tmp_list.erase(it);
			}else
				++it;
		}
		
		// /* merge the 2 sorted lists */
		main_list->merge(tmp_list);   
	}

	return SUCCESS;
}

bool 
Statistics::isValidIdxs(int i, int j){ return i>=0 && i<N_ && j>=0 && j<N_;}