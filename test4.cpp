#include <iostream>
#include <list>
#include <algorithm>
#include <map>
#include <string>
#include "library1.h"

using std::string;
using std::cout;
using std::endl;
using std::lower_bound;
using std::make_pair;
using std::ostream;
using std::list;
using std::pair;
using std::map;


class Fruit {
  private:
    int    i_;
    int    j_;
    int    rape_rate_;
    int    fruit_ID_;

  public:
    Fruit(int i, int j, int id, int rate) : 
          i_(i), j_(j), fruit_ID_(id), rape_rate_(rate) {}
    void   setID(int id) {fruit_ID_ = id;}
    void   setRate(int rate) {rape_rate_ = rate;}
    int    getID() {return fruit_ID_;}
    int    getRate() {return rape_rate_;}
    int    getRow() {return i_;}
    int    getCol() {return j_;}

    bool operator == (const Fruit& other) const {return fruit_ID_ == other.fruit_ID_;}
    bool operator != (const Fruit& other) const {return fruit_ID_ != other.fruit_ID_;}
    bool operator < (const Fruit& other) const { 
      if (rape_rate_ == other.rape_rate_)
        return fruit_ID_ < other.fruit_ID_;
      return rape_rate_ < other.rape_rate_;
    }
    bool operator > (const Fruit& other) const { 
      if (rape_rate_ == other.rape_rate_)
        return fruit_ID_ > other.fruit_ID_;
      return rape_rate_ > other.rape_rate_;
    }
    friend ostream& operator << (ostream& os, const Fruit& f);
};
ostream& operator << (ostream& os, const Fruit& f){
  os << "{idx: (" << f.i_ << ", " << f.j_ << ")" <<
        " id: " << f.fruit_ID_ << 
        ", Rate: " << f.rape_rate_ << "}";
  return os;
}

class TreeFruits {
  private:
    int N_;
    int i_, j_;
    int num_of_fruit_;
    list<Fruit> fruits_;

  public:
    TreeFruits(int N, int i, int j) : 
          N_(N), i_(i), j_(j), num_of_fruit_(0) {}
    ~TreeFruits(){
      for (auto it = fruits_.begin(); it != fruits_.end(); ++it){
        Fruit& p = *it;
        delete &p;
      }
    }
    list<Fruit>* getFruitsList() {return &fruits_;}
    Fruit* getBestFruit() {return &(fruits_.front());}
    int  getNumOfFruits() {return num_of_fruit_;}
    void increaseNumOfFruits() {  num_of_fruit_++;}
    void decreaseNumOfFruits() {num_of_fruit_--;}
};

class Tree {
  private:
    int           N_;
    int           i_, j_;
    TreeFruits*   tree_fruits_;

  public:
    Tree(int N, int i, int j, TreeFruits* ptr = nullptr) : 
          N_(N), i_(i), j_(j), tree_fruits_(ptr) {}
    ~Tree(){ delete tree_fruits_; }
    TreeFruits* getTreeFruitsPtr() {return tree_fruits_;}
    void        setTreeFruitsPtr(TreeFruits* ptr){ tree_fruits_ = ptr;}

    friend ostream& operator << (ostream& os, const Tree& f);
};
ostream& operator << (ostream& os, const Tree& t){
  os << "Tree idx: " << t.i_ << ", " << t.j_ ;
  return os;
}

class Statistics {
  private:
    int N_;
    map<int/* i*N+j */, Tree*> trees_; // log(n)
    map<int/* i*N+j */, TreeFruits*> all_trees_fruit_list_; // log(k)

  public:
    Statistics(int N) : N_(N) {}
    ~Statistics() {
      /* delete trees_ */
      for (auto& pair : trees_){
        pair.second->~Tree();
        pair.second = nullptr;
      }
      /* delete all_trees_fruit_list_ */
      for (auto& pair : all_trees_fruit_list_)
        pair.second = nullptr;
    }
    map<int,Tree*>        getTrees(){return trees_;}
    map<int,TreeFruits*>  getFruits(){return all_trees_fruit_list_;}
    StatusType            plantTree(int i, int j){

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
    StatusType            addFruit(int i, int j, int id, int rate) {
      
      // invalid input
      if (!isValidIdxs(i, j) || rate <= 0 || id <= 0) return INVALID_INPUT;

      /* if tree not found return FALUIRE */
      auto key_tree_pair = trees_.find(i*N_+j);
      if (key_tree_pair == trees_.end()) 
        return FAILURE;
      
      /* creat new fruit. if if faild return ALLOCATION_ERROR */
      Fruit* new_fruit = nullptr;
      try{ new_fruit = new Fruit(i, j, id, rate);}
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
    StatusType            pickFruit(int id) {
      
      /* invalid input */
      if (id <= 0) return INVALID_INPUT;

      /* finding the fruit */
      for (auto& key_treeFruit_pair :all_trees_fruit_list_){
        list<Fruit>* fruit_list = key_treeFruit_pair.second->getFruitsList();
        auto it = find_if(fruit_list->begin(), fruit_list->end(),
                          [&id](Fruit& f){ return f.getID() == id;});
        // fruit found - remove
        if (it != fruit_list->end()){
          fruit_list->erase(it);
          key_treeFruit_pair.second->decreaseNumOfFruits(); 
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
    StatusType            rateFruit(int id, int rate) {

      /* invalid input */
      if (id <= 0 || rate <= 0) return INVALID_INPUT;

      /* finding the fruit, creat updated fruit, removing old, inserting back*/
      for (auto& key_treeFruit_pair :all_trees_fruit_list_){
        list<Fruit>* list = key_treeFruit_pair.second->getFruitsList();
        auto it = find_if(list->begin(), list->end(),
                          [&id](Fruit& f){ return f.getID() == id;});

        if (it != list->end()){
          // create updated copy of the fruit
          Fruit* updated_fruit = new Fruit(it->getRow(), it->getCol(),
                                       it->getID(), rate);
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
    StatusType            getBestFruit(int i, int j, int* fruitID) {

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
    StatusType            getAllFruitsByRate(int i, int j, int **fruits, int *numOfFruits) {

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
    StatusType            updateRottenFruits(int rottenBase, int rottenFactor) {
       /* invalid input */
      if (rottenBase < 1 || rottenFactor < 1) 
        return INVALID_INPUT;
      
      // if factor == 1 nothing change.
      if (rottenFactor == 1) return SUCCESS;

      for (auto key_treeFruits_pair : all_trees_fruit_list_){
        list<Fruit>* main_list = key_treeFruits_pair.second->getFruitsList();
        list<Fruit> tmp_list(*main_list);

        /* update all rotten fruits and erase the rest */
        for (auto it = main_list->begin(); it != main_list->end(); ++it){
          
          int id = it->getID();
          int rate = it->getRate();
          if (id % rottenBase == 0){
            it->setRate(it->getRate() * rottenFactor);
            rate = it->getRate();
          }else
            main_list->erase(it);
        }

        // /* erase all the rotten fruits */
        for (auto it = tmp_list.begin(); it != tmp_list.end(); ++it)
          if (it->getID() % rottenBase == 0)
            tmp_list.erase(it);
        
        // /* merge the 2 sorted lists */
        main_list->merge(tmp_list);   
        // for (auto& fruit : *main_list) cout << fruit << endl; cout << endl;
      }

      return SUCCESS;
    }
    void printAllFruits(){
      cout << "\nhere are all the trees with their fruits sorted";
      for (const auto& key_treeFruits_pair :all_trees_fruit_list_){
        // cout << key_treeFruits_pair.second.size() << endl;
        cout << "\ntree idx: " << 
        keyToIndexString(key_treeFruits_pair.first) << endl;
        for(const auto& fruit : *(key_treeFruits_pair.second->getFruitsList())){
          cout << fruit << endl;
        }
      }
    }

  private:
    bool isValidIdxs(int i, int j){ return i >= 0 && i < N_ && 
                                           j >= 0 && j < N_; }
    string keyToIndexString(int key){
      string i_str = std::to_string(key / N_);
      string j_str = std::to_string(key % N_);
      return "{" + i_str + ", " + j_str + "}";
    }
};

string msg(StatusType s){
  if (s == SUCCESS) return "SUCCESS";
  if (s == FAILURE) return "FAILURE";
  if (s == ALLOCATION_ERROR) return "ALLOCATION_ERROR";
  if (s == INVALID_INPUT) return "INVALID_INPUT";
}

void printArr(int* arr, int size){
  cout << "{";
  for (int i = 0; i < size; i++){
    if (i < size-1)
      cout << arr[i] << ", ";
    else
      cout << arr[i];
  }
  cout << "}" << endl;
}

int main () {
  Statistics* field = new Statistics(10);
  field->plantTree(1,2);
  field->addFruit(1,2,193, 7);
  field->addFruit(1,2,190, 8);
  field->addFruit(1,2,10, 1);
  field->addFruit(1,2,20, 2);
  field->addFruit(1,2,50, 5);
  field->updateRottenFruits(10, 10);
  field->printAllFruits();
}

// int main() {
//   cout << "\n\n******************************" << endl;
//   cout <<     "******* planting Trees *******" << endl;
//   cout <<     "******************************" << endl;
//   Statistics* field = new Statistics(10);
//   cout << msg(field->plantTree(5,2)) << "\t\tshould be: SUCCESS "              << endl;
//   cout << msg(field->plantTree(2,4)) << "\t\tshould be: SUCCESS "              << endl;
//   cout << msg(field->plantTree(3,9)) << "\t\tshould be: SUCCESS "              << endl;
//   cout << msg(field->plantTree(4,4)) << "\t\tshould be: SUCCESS "              << endl;
//   cout << msg(field->plantTree(2,7)) << "\t\tshould be: SUCCESS "              << endl;
//   cout << msg(field->plantTree(4,4)) << "\t\tshould be: FAILURE tree exist"    << endl;
//   cout << msg(field->plantTree(4,40))<< "\tshould be: INVALID_INPUT "        << endl;cout << endl;
//   cout <<"these are the trees:" << endl;
//   for (auto tree_pair : field->getTrees()) cout << *(tree_pair.second) << endl;cout<<endl;


//   cout << "\n\n******************************" << endl;
//   cout <<     "*******    addFruit    *******" << endl;
//   cout <<     "******************************" << endl;
//   cout << "tree: 2,4" << endl;
//   cout << msg(field->addFruit(2,4,127,2)) << "\t\tshould be: SUCCESS "             << endl; 
//   cout << msg(field->addFruit(2,4,121,2)) << "\t\tshould be: SUCCESS "             << endl; 
//   cout << msg(field->addFruit(2,4,121,1)) << "\t\tshould be: FAILURE -> same id "  << endl;
//   cout << msg(field->addFruit(2,9,125,4)) << "\t\tshould be: FAILURE -> no tree"   << endl; 
//   cout << msg(field->addFruit(2,4,135,4)) << "\t\tshould be: SUCCESS "             << endl; cout<<endl; 

//   cout << "tree: 5,2" << endl;
//   cout << msg(field->addFruit(7,2,23,1))  << "\t\tshould be: FAILURE -> unknown tree " << endl;
//   cout << msg(field->addFruit(5,2,21,2))  << "\t\tshould be: SUCCESS "                 << endl; 
//   cout << msg(field->addFruit(11,2,27,3)) << "\tshould be: INVALID_INPUT"              <<endl;
//   cout << msg(field->addFruit(5,2,25,4))  << "\t\tshould be: SUCCESS"                  << endl; 
//   cout << msg(field->addFruit(5,2,35,4))  << "\t\tshould be: SUCCESS "                 << endl; cout << endl;
  
//   cout << "tree: 4,4" << endl;
//   cout << msg(field->addFruit(7,6,23,1))  << "\t\tshould be: FAILURE -> unknown tree "  << endl;
//   cout << msg(field->addFruit(4,4,265,4)) << "\t\tshould be: SUCCESS "                  << endl;
//   cout << msg(field->addFruit(4,4,267,3)) << "\t\tshould be: SUCCESS"                   << endl;
//   cout << msg(field->addFruit(4,4,261,2)) << "\t\tshould be: SUCCESS"                   << endl;
//   cout << msg(field->addFruit(4,4,365,2)) << "\t\tshould be: SUCCESS "                  << endl; cout<<endl;
//   field->printAllFruits();cout << endl;


//   cout << "\n\n******************************" << endl;
//   cout <<     "*******   pickFruits   *******" << endl;
//   cout <<     "******************************" << endl;
//   cout << msg(field->pickFruit(121)) << "\t\tshould be: SUCCESS "                         << endl;
//   cout << msg(field->pickFruit(222)) << "\t\tshould be: FAILURE unknown id"               << endl;
//   cout << msg(field->pickFruit(267)) << "\t\tshould be: SUCCESS"                          << endl;
//   cout << msg(field->pickFruit(265)) << "\t\tshould be: SUCCESS"                          << endl;
//   cout << msg(field->pickFruit(261)) << "\t\tshould be: SUCCESS "                         << endl;
//   cout << msg(field->pickFruit(125)) << "\t\tshould be: FAILURE was not any tree in first"<< endl;
//   cout << msg(field->pickFruit(-2))  << "\tshould be: INVALID_INPUT"                     << endl; 
//   cout << msg(field->pickFruit(365)) << "\t\tshould be: SUCCESS "                         << endl; cout<<endl;

//   cout << msg(field->addFruit(4,4,265,4)) << "\t\tshould be: SUCCESS "                  << endl;
//   cout << msg(field->addFruit(4,4,267,3)) << "\t\tshould be: SUCCESS"                   << endl;
//   field->printAllFruits();cout << endl;


//   cout << "\n\n******************************" << endl;
//   cout <<     "*******   rateFruits   *******" << endl;
//   cout <<     "******************************" << endl;
//   cout << msg(field->rateFruit(121, 5))  << "\t\tshould be: FAILURE fruit removed" << endl;
//   cout << msg(field->rateFruit(127, 5))  << "\t\tshould be: SUCCESS"               << endl;
//   cout << msg(field->rateFruit(21, 3))   << "\t\tshould be: SUCCESS"               << endl;
//   cout << msg(field->rateFruit(265, 1))  << "\t\tshould be: SUCCESS"               << endl;
//   cout << msg(field->rateFruit(1221, 5)) << "\t\tshould be: FAILURE no fruit"      << endl;
//   cout << msg(field->rateFruit(-2, 5))   << "\tshould be: INVALID_INPUT"           << endl; cout<<endl;
//   field->printAllFruits();cout << endl;

//   cout << "\n\n******************************" << endl;
//   cout <<     "***   updateRottenFruits   ***" << endl;
//   cout <<     "******************************" << endl;
//   cout << msg(field->updateRottenFruits(84, 4))  << "\t\tshould be: SUCCESS no fruits" << endl;
//   // cout << msg(field->updateRottenFruits(1, 10))  << "\t\tshould be: SUCCESS"               << endl;
//   // cout << msg(field->updateRottenFruits())   << "\t\tshould be: SUCCESS"               << endl;
//   // cout << msg(field->updateRottenFruits())  << "\t\tshould be: SUCCESS"               << endl;
//   // cout << msg(field->updateRottenFruits()) << "\t\tshould be: FAILURE no fruit"      << endl;
//   // cout << msg(field->updateRottenFruits())   << "\tshould be: INVALID_INPUT"           << endl; cout<<endl;
//   field->printAllFruits();cout << endl;

//   cout << "\n\n******************************" << endl;
//   cout <<     "*******  getBestFruit  *******" << endl;
//   cout <<     "******************************" << endl;
//   int t52; int t12; int t24; int t39; int t44; int t74; int t27;
//   cout << msg(field->getBestFruit(5,2,&t52)) << "\t\tshould be: SUCCESS"                  << "  tree {5,2}: " << t52 << endl; 
//   cout << msg(field->getBestFruit(1,2,nullptr)) << "\tshould be: INVALID_INPUT nullptr"   << "  tree {1,2}: nullptr"  << endl;
//   cout << msg(field->getBestFruit(11,4,&t24))<< "\tshould be: INVALID_INPUT"              << "  tree {2,4}: "   << t24 << endl;
//   cout << msg(field->getBestFruit(3,9,&t39)) << "\t\tshould be: SUCCESS no fruit"         << "  tree {3,9}: " << t39 << endl;
//   cout << msg(field->getBestFruit(4,4,&t44)) << "\t\tshould be: SUCCESS"                  << "  tree {4,4}: " << t44 << endl;
//   cout << msg(field->getBestFruit(7,4,&t44)) << "\t\tshould be: FAILURE -> unknown tree " << "  tree {7,4}: " << t74 << endl;
//   cout << msg(field->getBestFruit(2,7,&t27)) << "\t\tshould be: SUCCESS no fruit"         << "  tree {2,7}: " << t27 << endl;

//   cout << "\n\n******************************" << endl;
//   cout <<     "****  GetAllFruitsByRate  ****" << endl;
//   cout <<     "******************************" << endl;
//   int* arr_t52; int* arr_t12; int* arr_t24; int* arr_t39; int* arr_t44; int* arr_t74; int* arr_t27;
//   int num_t52; int num_t12; int num_t24; int num_t39; int num_t44; int num_t27; int num_t74;
//   cout << msg(field->getAllFruitsByRate(5,2,&arr_t52, &num_t52)) << "\t\tshould be: SUCCESS"              "  tree {5,2} \t\t\t\tarr: "     ; printArr(arr_t52, num_t52) ; 
//   cout << msg(field->getAllFruitsByRate(1,2,nullptr, &num_t12))  << "\tshould be: INVALID_INPUT nullptr"  "  tree {1,2}:  \t\t" ; cout << endl;
//   cout << msg(field->getAllFruitsByRate(3,9,&arr_t39, &num_t39)) << "\t\tshould be: SUCCESS no fruit"     "  tree {3,9}: \t\tarr: "    ; printArr(arr_t39, num_t39) ;
//   cout << msg(field->getAllFruitsByRate(4,4,&arr_t44, &num_t44)) << "\t\tshould be: SUCCESS"              "  tree {4,4}: \t\t\tarr: "    ; printArr(arr_t44, num_t44) ;
//   cout << msg(field->getAllFruitsByRate(2,4,&arr_t24, &num_t24)) << "\t\tshould be: SUCCESS"              "  tree {2,4}: \t\t\tarr: "    ; printArr(arr_t24, num_t24) ;
//   cout << msg(field->getAllFruitsByRate(7,4,&arr_t74, &num_t74)) << "\t\tshould be: FAILURE no tree"      "  tree {7,4}: \t\tarr: "  ; cout << endl;
//   cout << msg(field->getAllFruitsByRate(2,7,&arr_t27, &num_t27)) << "\t\tshould be: SUCCESS no fruit"     "  tree {2,7}: \t\tarr: "    ; printArr(arr_t27, num_t27) ;



//   delete field;
//   field = nullptr;


//   return 0;
// }
