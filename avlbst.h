#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child);
    void removeFix(AVLNode<Key, Value>* node, int dif);
    void rotateLeft(AVLNode<Key, Value>* key);
    void rotateRight(AVLNode<Key, Value>* key);
    
    

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item) {
    //TODO

    if(this->root_ == nullptr) {
        AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        this->root_ = newNode;
        return;
    }
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;
    
    while (current != nullptr) {
        parent = current;
        if (new_item.first < current->getKey()) {
            current = current->getLeft();
        } else if (new_item.first > current->getKey()) {
            current = current->getRight();
        } else {
            current->setValue(new_item.second);
            return; 
        }
    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    newNode->setBalance(0);

    if (parent == nullptr) {
        this->root_ = newNode;
    } else if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
        if(parent->getBalance() == 1 || parent->getBalance() == -1) {
            parent->setBalance(0);
        } else if(parent->getBalance() == 0) {
            parent->updateBalance(-1);
            insertFix(parent, newNode);
        }
    } else {
        parent->setRight(newNode);
        if(parent->getBalance() == 1 || parent->getBalance() == -1) {
            parent->setBalance(0);
        } else if(parent->getBalance() == 0) {
            parent->updateBalance(1);
            insertFix(parent, newNode);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child) {
    //TODO
    if (parent->getParent() == nullptr || parent == nullptr) return;
    AVLNode<Key, Value>* gpnode = parent->getParent();
    if(parent == gpnode->getLeft()) {
        gpnode->updateBalance(-1);

        if(gpnode->getBalance() == 0) return;

        else if(gpnode->getBalance() == -1) {
            insertFix(gpnode, parent);
        } else if(gpnode->getBalance() == -2) {
            if(parent->getLeft() == child) {
                rotateRight(gpnode);
                parent->setBalance(0);
                gpnode->setBalance(0);
            }
            if(parent->getRight() == child) {
                rotateLeft(parent);
                rotateRight(gpnode);
                if(child->getBalance() == -1) {
                    parent->setBalance(0);
                    gpnode->setBalance(1);
                    child->setBalance(0);
                }
                else if(child->getBalance() == 0) {
                    parent->setBalance(0);
                    gpnode->setBalance(0);
                    child->setBalance(0);
                }
                else if(child->getBalance() == 1) {
                    parent->setBalance(-1);
                    gpnode->setBalance(0);
                    child->setBalance(0);
                }
            }
            
        }

    }
    if(parent == gpnode->getRight()) {
        gpnode->updateBalance(1);
        if(gpnode->getBalance() == 0) {
            return;
        } else if(gpnode->getBalance() == 1) {
            insertFix(gpnode, parent);
        } else if(gpnode->getBalance() == 2) {
            if(parent->getRight() == child) {
                rotateLeft(gpnode);
                parent->setBalance(0);
                gpnode->setBalance(0);
            }
            if(parent->getLeft() == child) {
                rotateRight(parent);
                rotateLeft(gpnode);
                if(child->getBalance() == 1) {
                    parent->setBalance(0);
                    gpnode->setBalance(-1);
                    child->setBalance(0);
                } else if(child->getBalance() == 0) {
                    parent->setBalance(0);
                    gpnode->setBalance(0);
                    child->setBalance(0);
                } else if(child->getBalance() == -1) {
                    parent->setBalance(1);
                    gpnode->setBalance(0);
                    child->setBalance(0);
                }
            } 
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node) {
    //TODO

    AVLNode<Key, Value>* rNode = node->getRight(); 
    node->setRight(rNode->getLeft());
    if (rNode->getLeft() != nullptr) {
        rNode->getLeft()->setParent(node);
    }
    
    rNode->setParent(node->getParent());

    if (node->getParent() == nullptr) { 
        this->root_ = rNode;
    } else if (node == node->getParent()->getLeft()) { 
        node->getParent()->setLeft(rNode);
    }  else { 
        node->getParent()->setRight(rNode);
    }

    rNode->setLeft(node);
    node->setParent(rNode);


}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node) {
    //TODO
    AVLNode<Key, Value>* lfNode = node->getLeft(); 
         
    node->setLeft(lfNode->getRight());
    if (lfNode->getRight() != nullptr) {
        lfNode->getRight()->setParent(node);
    }
    lfNode->setParent(node->getParent());
    if (node->getParent() == nullptr) { 
        this->root_ = lfNode;
    } 
    else if (node == node->getParent()->getLeft()) {
        node->getParent()->setLeft(lfNode);
    } 
    else { 
        node->getParent()->setRight(lfNode);
    }
    lfNode->setRight(node);
    node->setParent(lfNode);


}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
    //TODO
    AVLNode<Key, Value>* rmvNode = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (rmvNode == nullptr) return; 
    int dif = 0;
    if (rmvNode->getRight() && rmvNode->getLeft()) {
        AVLNode<Key, Value>* predecessor = static_cast<AVLNode<Key, Value>*>(this->predecessor(rmvNode));
        nodeSwap(rmvNode, predecessor);
    }

    AVLNode<Key, Value>* parent = rmvNode->getParent();

    AVLNode<Key, Value>* child = nullptr;
    if (rmvNode->getLeft() != nullptr) {
        child = rmvNode->getLeft();
        dif = 1; 
    } else if (rmvNode->getRight() != nullptr) {
        child = rmvNode->getRight();
        dif = -1; 
    }
    if (rmvNode == this->root_) {
        this->root_ = child;
        if (child) {
            child->setParent(nullptr);
        }
    } else {
        if (rmvNode == parent->getLeft()) {
            parent->setLeft(child);
            dif = 1;
        } else {
            parent->setRight(child);
            dif = -1;
        } if (child) {
            child->setParent(parent);
        }
    }

    delete rmvNode; 
    removeFix(parent, dif);
}


template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* child, int dif) {
    //TODO
    if (child == nullptr) return; 

    AVLNode<Key, Value>* parent = child->getParent();
    int nextDif;
    if(parent != nullptr) {
        if(child == parent->getLeft()) {
            nextDif = 1;
        }
        else if (child == parent->getRight()) {
            nextDif = -1;
        }
    }
    if (dif == -1) {
        if(child->getBalance() + dif == -2) {
        AVLNode<Key, Value>* cNew = child->getLeft();
        if(cNew->getBalance() == -1) {
            rotateRight(child);
            child->setBalance(0);
            cNew->setBalance(0);
            removeFix(parent, nextDif);
        } else if(cNew->getBalance() == 0) {
            rotateRight(child);
            child->setBalance(-1);
            cNew->setBalance(1);
        } else if(cNew->getBalance() == 1) { 
            AVLNode<Key, Value>* gpNode = cNew->getRight();
            rotateLeft(cNew);
            rotateRight(child);
            if(gpNode->getBalance() == 1) {
                child->setBalance(0);
                cNew->setBalance(-1);
                gpNode->setBalance(0);
            } else if(gpNode->getBalance() == 0) {
                child->setBalance(0);
                cNew->setBalance(0);
                gpNode->setBalance(0);
            } else if(gpNode->getBalance() == -1) {
                child->setBalance(1);
                cNew->setBalance(0);
                gpNode->setBalance(0);
            }

            removeFix(parent, nextDif);
        }
    } else if(child->getBalance() + dif == -1) {
            child->setBalance(-1);
        }
        else if(child->getBalance() + dif == 0) {
            child->setBalance(0);
            removeFix(parent, nextDif);
        }
    }
    
    else {
        if(child->getBalance() + dif == 2) {
            AVLNode<Key, Value>* cNew = child->getRight();
            if(cNew->getBalance() == 1) { 
                rotateLeft(child);
                child->setBalance(0);
                cNew->setBalance(0);
                removeFix(parent, nextDif);
            } else if(cNew->getBalance() == 0) {
                rotateLeft(child);
                child->setBalance(1);
                cNew->setBalance(-1);
            } else if(cNew->getBalance() == -1) { 
                AVLNode<Key, Value>* gpNode = cNew->getLeft();
                rotateRight(cNew);
                rotateLeft(child);
                if(gpNode->getBalance() == -1) {
                    child->setBalance(0);
                    cNew->setBalance(1);
                    gpNode->setBalance(0);
                } else if(gpNode->getBalance() == 0) {
                    child->setBalance(0);
                    cNew->setBalance(0);
                    gpNode->setBalance(0);
                } else if(gpNode->getBalance() == 1) {
                    child->setBalance(-1);
                    cNew->setBalance(0);
                    gpNode->setBalance(0);
                }

                removeFix(parent, nextDif);
            }
        } else if(child->getBalance() + dif == 1) {
            child->setBalance(1);
        } else if(child->getBalance() + dif == 0) {
            child->setBalance(0);
            removeFix(parent, nextDif);
        }
    }

    

}



template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif