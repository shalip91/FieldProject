#ifndef STATISTICS_H
#define STATISTICS_H
#include "library1.h"
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

class Fruit {
  public:
    int    i_;
    int    j_;
    int    rape_rate_;
    int    fruit_ID_;

  public:
    Fruit(int i, int j, int id, int rate);
    void   setID(int id);
    void   setRate(int rate);
    int    getID();
    int    getRate();
    int    getRow();
    int    getCol();

    bool operator == (const Fruit& other) const;
    bool operator != (const Fruit& other) const;
    bool operator < (const Fruit& other) const;
    bool operator > (const Fruit& other) const;

    friend ostream& operator << (ostream& os, const Fruit& f);
};



class TreeFruits {
  private:
    int N_;
    int i_, j_;
    int num_of_fruit_;
    list<Fruit> fruits_;

  public:
    TreeFruits(int N, int i, int j);
    ~TreeFruits();
    list<Fruit>*  getFruitsList();
    Fruit*        getBestFruit();
    int           getNumOfFruits();
    void          increaseNumOfFruits();
    void          decreaseNumOfFruits();
};


class Tree {
  private:
    int           N_;
    int           i_, j_;
    TreeFruits*   tree_fruits_;

  public:
    Tree(int N, int i, int j, TreeFruits* ptr = nullptr);
    ~Tree();
    TreeFruits* getTreeFruitsPtr();
    void        setTreeFruitsPtr(TreeFruits* ptr);
};


class Statistics {
  private:
    int N_;
    map<int/* i*N+j */, Tree*> trees_; 
    map<int/* i*N+j */, TreeFruits*> all_trees_fruit_list_;

  public:
    Statistics(int N);
    ~Statistics();
    map<int,Tree*>        getTrees();
    map<int,TreeFruits*>  getFruits();
    StatusType            plantTree(int i, int j);
    StatusType            addFruit(int i, int j, int id, int rate);
    StatusType            pickFruit(int id);
    StatusType            rateFruit(int id, int rate);
    StatusType            getBestFruit(int i, int j, int* fruitID);
    StatusType            getAllFruitsByRate(int i, int j, int **fruits, int *numOfFruits);
    StatusType            updateRottenFruits(int rottenBase, int rottenFactor);
    
  private:
    bool isValidIdxs(int i, int j);
};
#endif // STATISTICS_H
