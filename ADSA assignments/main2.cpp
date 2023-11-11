#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

class AVLTree{
private:

    // struct for Node
    struct Node{
        int item;
        int height;
        int bf; // balanced factor
        Node* left, *right;
        Node(int item){
            this->item = item;
            left = nullptr;
            right = nullptr;
            height = -1;
            bf = 0;
        }
    };
    Node * root;

    int height( Node * root ){ 
        // height = #edges from root -> furthest leaf node (i.e at root-> height = -1)
        int left_h = -1;
        int right_h = -1;

        if ( root == nullptr ){
            return -1;
        }else{
            left_h = height(root->left);
            right_h = height(root->right);
        }
        return 1 + std::max(left_h, right_h);
    }

    int balanced_factor( Node * root ){
        // balanced factor = height_left_subtree - height_right_subtree
        if ( root == nullptr ){
            return -1;
        }
        return height(root->left) - height(root->right);
    }

    Node * left_rotate( Node * root ){ // For left-left imbalance case: abs(bf) > 1
        Node* new_parent = root->right;
        root->right = new_parent->left;
        new_parent->left = root;

        // update height and bf factor
        update_node(root);
        update_node(new_parent);

        return new_parent;
    }

    Node * right_rotate( Node * root ){ // For right-right imbalance case: abs(bf) > 1
        Node* new_parent = root->left;
        root->left = new_parent->right;
        new_parent->right = root;

        // update height and bf factor
        update_node(root);
        update_node(new_parent);

        return new_parent;
    }

    Node * left_right_rotate( Node * root){ // For left-right imbalance case: abs(bf) > 1
        root->left = left_rotate(root->left);
        return right_rotate(root);
    }

    Node * right_left_rotate( Node * root ){ // For right-left imbalance case: abs(bf) > 1
        root->right = right_rotate(root->right);
        return left_rotate(root);
    }

    void update_node( Node* root ){
        int left_h = -1;
        int right_h = -1;

        // calculate height of left and right subtree
        // -> get height of current node's height
        if ( root->left != nullptr ){
            left_h = root->left->height;
        }
        if ( root->right != nullptr ){
            right_h = root->right->height;
        }

        // update current node's height (aka root in parameter)
        root->height = 1 + std::max(left_h, right_h);
        // update current's node balanced factor
        root->bf = left_h - right_h;
    }
    
    
    Node * balance_tree( Node * root ){
        // handle binary search tree's imbalance -> bff at every node = {-1,0,1}
        // Note: every time we rotate, we update the height and bf properties of each node

        // left-heavy tree (bf >= 2)
        if ( root->bf >= 2 ){
            // left-left imbalance case
            if ( root->left->bf >= 0 ){ 
                return right_rotate(root);
            }
            // left-right imbalance case
            else{       
                return left_right_rotate(root);
            }
        }

        // right-heavy tree (bf <= -2) 
        if ( root->bf <= -2 ){
            // right-right imbalance case
            if ( root->right->bf <=0 ){ 
                return left_rotate(root); 
            }
            // right-left imbalance case
            else{       
                return right_left_rotate(root);
            }
        }
        // if bf = {-1,0,1} -> already balanced -> do nothing
        return root;
    }

    Node * insert_node( Node* &root, int item ){
        // if tree is empty
        if ( root == nullptr ){
            root = new Node(item);
        }
        // else, tree not empty
        else{
            // if item < root -> traverse left subtree
            if ( item < root->item ){
                root->left =  insert_node(root->left, item);
            }
            // if item > root -> traverse right subtree
            else if ( item > root-> item) {
                root->right =  insert_node(root->right, item);
            }
            // else, if item already exists -> do nothing
        }
        // update newly inserted node's height and bf properties
        update_node(root);
        // balance tree
        return balance_tree(root);
    }

    // find max in tree
    Node * find_max ( Node* root ){
        if ( root == nullptr ){
            return nullptr;
        }
        Node * max = root;
        while ( max->right != nullptr ) {
            max = max -> right;
        }
        return max;
    }

    Node * delete_node( Node* root, int target ){
        // is tree is empty
        if ( root == nullptr ){
            return nullptr;
        }
        // travserse left subtree
        else if ( target < root->item ){
            root->left = delete_node(root->left, target);
        }
        // traverese right subtree
        else if ( target > root->item  ){
            root->right = delete_node(root->right, target);
        }
        // else found the node to delete
        else{
            // case 1 & 2:
                // case 1: node to delete has no children
                // case 2: node to delete has either left or right child
            if ( root->left == nullptr || root->right == nullptr ) {
                Node * sub = root->left ? root->left : root->right;
                delete root;
                return sub;
            }

            // case 3: node to delete has 2 children
            if ( root->left != nullptr && root->right != nullptr ) {
                // find max node of left subtree
                Node * sub = find_max(root->left);
                // replace of value of max of left subtree with found node's value
                root->item = sub->item;
                // delete duplicate values
                root->left = delete_node(root->left, sub->item);
            }
        }
        // update node's height and balanced factor
        update_node(root);
        // balance tree
        return balance_tree(root);        
    }

    void pre_order( Node * root ){
        if ( root == nullptr ){
            return;
        }
        cout << root->item << " ";
        pre_order(root->left);
        pre_order(root->right);
    }

    void in_order( Node * root ){
        if ( root == nullptr ){
            return;
        }
        in_order(root->left);
        cout << root->item << " ";
        in_order(root->right);
    }

    void post_order( Node * root ){
        if ( root == nullptr ){
            return;
        }
        post_order(root->left);
        post_order(root->right);
        cout << root->item << " ";
    }    
    
    // print 2D (graphical print)
    void print_2D(Node* root, int space){
        if (root == nullptr)
            return;
        space += 5;
        print_2D(root->right, space);
        cout<<endl;
        for (int i=5; i<space; i++) 
            cout <<" ";
        cout<<root->item<<endl;
        print_2D(root->left,space);
    }
    

// ######################################################################################
    // ## HELPER FUNCTIONS
public:
    AVLTree(){
        root = nullptr;
    }

    int insert(int item){
        root = insert_node(root, item);
        return 1;
    }

    int del(int target){
        root = delete_node(root, target);
        if ( root != nullptr ) return 1;
        return 0;
    }

    void pre_order(){
        if ( root == nullptr ){
            cout << "EMPTY"<<endl;
            return;
        }else{
            pre_order(root);
            // cout << endl;
        }
    }

    void in_order(){
        if ( root == nullptr ){
            cout << "EMPTY"<<endl;
            return;
        }else{
            in_order(root);
            // cout << endl;
        }
    }

    void post_order(){
        if ( root == nullptr ){
            cout << "EMPTY"<<endl;
            return;
        }else{
            post_order(root);
            // cout << endl;
        }
    }

    void print_2D(){
        print_2D(root, 2);
        cout << "############################################" << endl;
    }
};

// helper function to parse input data then execute based on data
void execute(string cmd){
    AVLTree t;
    stringstream ss(cmd);  
    string temp;
    while (ss >> temp) { // each token is separated by a " "

        // insertion case
        if ( temp.at(0) == 'A'){
            string num = temp.substr(1, temp.length()-1);
            int item = stoi(num);
            t.insert(item);
        }
        // deletion case
        else if(temp.at(0) == 'D'){
            string num = temp.substr(1, temp.length()-1);
            int item = stoi(num);
            t.del(item);
        }
        // traversal case
        else{
            string option = temp;

            // pre-order
            if ( option == "PRE" ){
                t.pre_order();
            }
            // in-order
            else if ( option == "IN" ){
                t.in_order();
            }
            // post-order
            else if ( option == "POST" ){
                t.post_order();
            }
        } 
    }
}


int main(){


    // AVLTree t;
    // vector<int> entry = {20,10,30,25,40};
    // for (int i=0; i<entry.size(); i++){
    //     t.insert(entry.at(i));
    // }

    // t.print();

    // t.del(10);
    // t.print();
    // t.del(40);
    // t.print();

    // t.post_order();

    string data;
    getline(cin, data);
    execute(data);
 
    return 0;
}