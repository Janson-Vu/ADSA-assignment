#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;


class RedesignRoad{

private:
    struct Edge{
        int from, to, cost;
        Edge(int f, int t, int c){
            from = f; to = t; cost = c;
        }
    };

    // helps with sort later
    struct Asc {
        bool operator() (const Edge &node1, const Edge &node2) { 
            return node1.cost < node2.cost;
        }
    };
    // helps with sort later
    struct Desc {
        bool operator() (const Edge &node1, const Edge &node2) { 
            return node1.cost > node2.cost;
        }
    };

    // Union-find using disjoint set
    struct Disjoint_set{
        int nodes_num;

        // number of nodes in each set with specified id (id = index)
        vector<int> size;

        // id[i] points to the parent of i, if id[i] = i then i is a root node
        vector<int> id;

        // keep track of number of sets
        int sets_num;

        // constructor
        Disjoint_set(int sz){
            nodes_num = sz; sets_num = sz;

            // initialize Union Find size array
            size.resize(sz, 1); // each node has only itself as one of its member

            // initialize Union Find id array
            id.resize(sz);
            // in the beginning, each node points to itself
            for (int i=0; i<sz; i++) id[i] = i;
        }

        // Find method: Find which set 'p' belongs to
        int find( int p ){
            // find the root of the set
            int root = p;
            // keep finding if the node does not point to itself
            while ( root != id[root] ) root = id[root];
            return root;
        }

        // Connected: check if 'p' and 'q' belong to same set
        bool connected( int p, int q ){
            return find(p) == find(q) ? true: false;
        }

        // Unify method: Unify the sets containing elements 'p' and 'q'
        void unify ( int p, int q ){

            // if 'p' and 'q' in same set -> do nothing
            if ( connected(p,q) ) return;

            // else, do unify
            int root1 = find(p);
            int root2 = find(q);

            // Merge smaller set into the larger one
            if ( size[root1] < size[root2] ){
                size[root2] += size[root1];
                id[root1] = root2;
                size[root1] = 0;
            }
            else{
                size[root1] += size[root2];
                id[root2] = root1;
                size[root2] = 0;
            }

            // after merging, number of sets (in total) decreases by 1
            sets_num--;
        }
    };

    /* 
        2D matrices: 
        - country[i][j]=1 or 0: there is already a road between city i, j 
            if and only if country[i][j]=1

        - build[i][j]: the cost for building a road between i and j. 
            The values of build[i][j] =  [A->Z] = [0->25] OR [a->z] = [26->51]. 

        - destroy[i][j]: the cost for destroying a road between i and j. 
    */
    vector<vector<int>> init_matrix( string input ) { 
        // result 2D matrix
        vector<vector<int>> matrix;
        stringstream ss(input);
        char delim = ','; 
        string token;

        // get each token separated by ","
        while ( getline( ss, token, delim ) ){
            vector<int> row; 

            // construct 1D matrix from each token

            // if these tokens belong to country matrix
            if ( token.at(0) == '0' || token.at(0) == '1' ){
                for ( char c : token ) row.push_back(c - '0');
            }
            // if these tokens belong to build/destroy matrix
            else{
                for ( char c : token ) {
                    // uppercase
                    if ( c >= 'A' && c <= 'Z') {
                        row.push_back(c - 'A');
                    }
                    // lowercase
                    else{
                        row.push_back(c - 'a' + 26);
                    }
                }
            }   
            // add to the 2D matrix
            matrix.push_back(row);
        }
        return matrix;
    }

    vector<string> split_input( string input ){
        vector<string> segment;
        stringstream ss(input);
        char delim = ' '; 
        string token;

        while ( getline( ss, token, delim ) ){
            segment.push_back(token);
        }

        return segment;
    }
    
public:

    int redesign_cost( string input ){

        // STEP 1: parse input

        // split input into 3 parts 
        vector<string> segment = split_input(input);
        // parse each parts of input
        // segment[0] = connection
        vector<vector<int>> country = init_matrix(segment[0]); 
        // segment[1] = build cost
        vector<vector<int>> build   = init_matrix(segment[1]); 
        // segment[2] = destroy cost
        vector<vector<int>> destroy = init_matrix(segment[2]); 


        // STEP 2: use kruskal to find minimum spanning tree
        int n = country.size();

        // construct all possible edges ONCE ( edge(a,b) is edge(b,a) ). No self loop in here.
        // This generates edges like: (1,0, cost), (2,0, cost), (2,1, cost), (3,0, cost), etc
        // This sets up edges to run Kruskal.
        vector<Edge> destruct;
        vector<Edge> construct;

        for (int i=0; i<n; i++){
            for (int j=0; j<i; j++){
                // if road already build, might want to mark this edge for destroy
                if ( country[i][j] == 1 ){
                    destruct.push_back( Edge(i,j, destroy[i][j] ) );
                }
                // if road not existed, might want to mark this construction
                else{
                    construct.push_back( Edge(i,j, build[i][j] ));
                }
            }
        }

        // Set up disjoint sets for Kruskal
        Disjoint_set S(n);
        int total_cost = 0;
        
        // sort destruct in DESCENDING order
        // -> near the end of our destroy process, edges of same set exists a lot more
        // -> want to have cheapeast cost near the end 
        sort(destruct.begin(), destruct.end(), Desc());

        // do Kruskal -> connect the clustered cities together first
        // and find what road needs to be destroyed between a pair of cities in each cluster
        for ( Edge &road : destruct ){
            // if cities in cluster not connected -> connect
            if ( S.find(road.from) != S.find(road.to) ){
                S.unify(road.from, road.to);
            }
            // else, we have found the spare road -> add to destroy cost
            else{
                total_cost = total_cost + road.cost;
            }
        }        

        // sort construct in ASCENDING order
        // -> when starting building edges, want to have cheapest cost
        sort(construct.begin(), construct.end(), Asc());

        // do Kruskal on the clustered-ly connected cities
        // -> build road between each cluster
        for ( Edge &road : construct ){
            // if no road between a pair of cluster -> build one && add to cost
            if ( S.find(road.from) != S.find(road.to) ){
                S.unify(road.from, road.to);
                total_cost = total_cost + road.cost;
            }
        }
        return total_cost;
    }
};

void execute( string data ){
    RedesignRoad sol;
    cout << sol.redesign_cost(data) << endl;
}

int main(){

    string data;
    getline(cin, data);
    execute(data);

    // string tst1 = "000,000,000 ABD,BAC,DCA ABD,BAC,DCA";
    // string tst2 = "011,101,110 ABD,BAC,DCA ABD,BAC,DCA";
    // string tst3 = "011000,101000,110000,000011,000101,000110 ABDFFF,BACFFF,DCAFFF,FFFABD,FFFBAC,FFFDCA ABDFFF,BACFFF,DCAFFF,FFFABD,FFFBAC,FFFDCA";
    // string tst4 = "0 A A";
    // string tst5 = "0001,0001,0001,1110 AfOj,fAcC,OcAP,jCPA AWFH,WAxU,FxAV,HUVA";
    
    // cout << sol.redesign_cost(tst1) << endl;
    // cout << sol.redesign_cost(tst2) << endl;
    // cout << sol.redesign_cost(tst3) << endl;
    // cout << sol.redesign_cost(tst4) << endl;
    // cout << sol.redesign_cost(tst5) << endl;


    return 0;
}