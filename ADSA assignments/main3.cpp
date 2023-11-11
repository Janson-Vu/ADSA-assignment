#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

const string TOMBSTONE = "#";
const string UNUSED = "$";


class HashMap{
public:
    int size;
    int capacity;
    vector<string> table;

    int hash( string key ){
        return (key.back() - 97) % 26; // offset starts at 'a'    (ASCII a = 97)
    }


public:

    HashMap(){
        size = 26;
        capacity = 0;
        table.resize(size, UNUSED);        
    }

    // linear probing
    int linear_probe( int x ){ // x = current hash value

        // cout << "before" <<endl;
        while ( table.at(x) != UNUSED && table.at(x) != TOMBSTONE ){
            // cout << x << endl;
            x++;
            if ( x > size-1 ){
                x = 0;
            }
        }
        // cout << "after" << endl;
        return x;
    }

    // search
    int search( string key ){ // return -1 (does not exist), 0-25 (exists)

        int hash_val = hash(key);
        // if hash value invalid, outside [0,25] -> do nothing
        if (hash_val < 0 || hash_val > 25){
            return -1;
        }

        int index = hash_val;

        // if slot UNUSED 
        if ( table.at(index) == UNUSED ){
            // cout << "search: unused" <<endl;
            return -1;
        }
        // if slot is tombstone or occupied 
        else {
            // cout << "search: occupied" <<endl;

            // linear probe till we can find the key in the table, 
            // or we come back at the same starting key
            int original_index = index;

            // if occupied slot = target
            if ( table.at(index) == key){
                //cout << "found "<<key<<endl;
                return index;
            }

            // if occupied slot is NOT target
            // start looking at the next slot (if overbound, start at beginning )
            index++;
            if ( index > size-1){
                index = 0;
            }

            while ( table.at(index) != UNUSED ){
                // cout << index << endl;

                // if overbound, start at beginning again
                // if ( index > size-1 ){
                //     index = 0;
                // }

                // if found key 
                if ( table.at(index) == key ){
                    //cout << "found "<<key <<endl;
                    return index;
                }

                // if go whole cycle and cannot find key
                else if ( index == original_index ){
                    //cout << "NOT found "<<key <<endl;
                    return -1;
                }
                index++;

                if ( index > size-1 ){
                    index = 0;
                }
            }
            //cout << "NOT found "<<key <<endl;
            return -1;
        }

        return false;
    }

    // insert
    void insert( string key ){

        // if table is full OR
        // key already existed -> do nothing
        if ( capacity == size || search(key) != -1  ){ // problem is at search(key here)
            //cout << "-------------" <<endl;
            return;
        }


        int hash_val = hash(key);
        // if hash value invalid, outside [0,25] -> do nothing
        if (hash_val < 0 || hash_val > 25){
            //cout << "-------------" <<endl;
            return;
        }

        // cout << key <<" "<<hash_val<< endl;
        
        // unused case
        if ( table.at(hash_val) == UNUSED ){
            // cout << "insert: unused" <<endl;
            table.at(hash_val) = key;
        }

        // occupied case
        else if ( table.at(hash_val) != UNUSED && table.at(hash_val) != TOMBSTONE ){
            // cout << "insert: occupied" << endl;
            int index = linear_probe(hash_val);     
            table.at(index) = key;
        }

        // tombstone case
        else if ( table.at(hash_val) == TOMBSTONE ){
            // cout << "insert: tombstone" <<endl;
            int index = hash_val;
            table.at(index) = key;
        }

        capacity++;
        // cout << "-------------" <<endl;
    }

    // delete (this is still missing repair functionality)
    void remove( string key ){
        int index = search(key);

        if (index != -1){
            table.at(index) = TOMBSTONE;
        }
    }

    // print
    void print(){
        for ( string key : table ){
            if ( key != UNUSED && key != TOMBSTONE) 
                cout << key << " ";
        }
        cout << endl;
    }
};

void execute( string cmd ){
    HashMap t;
    stringstream ss(cmd);  
    string temp;

    while ( ss >> temp ){

        // insertion case
        if ( temp.at(0) == 'A' ){
            string key = temp.substr(1, temp.length()-1);
            t.insert(key);
        }

        // remove case
        else if(temp.at(0) == 'D'){
            string key = temp.substr(1, temp.length()-1);
            t.remove(key);
        }
    }

    // print out result
    t.print();
}


int main(){

    // HashMap t;

    // vector<string> input = {"zow", "qow", "mow", "cow", "vow", "sow", "ozz", "aoa", "coa", "zow", "qoz", "woz", "moz", "nozv"};
    // // Azow Aqow Amow Acow Avow Asow Aozz Aaoa Acoa Azow Aqoz Awoz Amoz Anozv Dssays Dtraditio Dsow Dsow
    // // expected: vow ozz aoa coa qoz woz moz nozv zow qow mow cow
    // // actual:   vow ozz aoa coa qoz woz moz nozv zow qow mow cow 

    // for (string in : input){
    //     t.insert(in);
    // }
    // t.print();

    
    // vector<string> del = {"ssays", "traditio", "sow", "sow"};
    // for (string in : del){
    //     t.remove(in);
    // }
    // t.print();

    string data;
    getline(cin, data);
    execute(data);

    return 0;
}