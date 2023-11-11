#include <list>
#include <string>
#include <iostream>
#include <math.h>
using namespace std;

string add(string num1, string num2, int base){

    string sum;
    int carry = 0;

    while( !num1.empty() || !num2.empty() || carry != 0 ){

        int atomic_sum = 0;

        // get the digit from the first integer
        int digit1 = 0;
        if ( !num1.empty() ){
            digit1 = num1.back() - '0';
            num1.pop_back();
        }

        // get digit from second integer
        int digit2 = 0;
        if ( !num2.empty() ){
            digit2 = num2.back() - '0';
            num2.pop_back();
        }

        // calculate the sum of first column of digits, and get the carry (if any) 
        atomic_sum = (digit1 + digit2 + carry) % base;
        carry = (digit1 + digit2 + carry) / base;
        
        // prepend result to final sum string
        sum.insert(0, 1, atomic_sum + '0');
    }

    return sum;
}

string subtract(string num1, string num2, int base){
    string diff;
    int borrow = 0;

    while( !num1.empty() || !num2.empty() || borrow != 0 ){

        int atomic_sub = 0;

        // get digit of first num
        int digit1 = 0;
        if ( !num1.empty() ){
            digit1 = num1.back() - '0';
            num1.pop_back();
        }

        // get digit of second num
        int digit2 = 0;
        if ( !num2.empty() ){
            digit2 = num2.back() - '0';
            num2.pop_back();
        }

        // calculate subtraction of a pair of 1-digit
        atomic_sub = (digit1 + borrow) - digit2;

        if ( atomic_sub < 0 ){
            atomic_sub += base;
            borrow = -1;
        }
        else{
            borrow = 0;
        }

        // prepend into result string
        diff.insert(0, 1, atomic_sub + '0');
    }

    // remove leading 0 
    while ( !diff.empty() && diff.at(0) == '0' ){
        diff.erase(0, 1);
    }

    // if result is all 0 -> we might have deleted all -> now we add 1 back
    if ( diff.empty() ){
        diff.push_back('0');
    }

    return diff;
}

// we switch to school method multiply (here) when n < 4
string mult(string num1, string num2, int base){
    string product = "0";

    // looping over each digit of num2 
    for (int i = num2.length()-1; i>=0; i--){

        string prod;
        int carry = 0;
        int digit2 = num2.at(i) - '0';
        

        // looping over each digit of num1
        for (int j = num1.length()-1; j>=0; j--){
            int digit1 = num1.at(j) - '0';
            // cout << "digit1 = "<<digit1 <<endl;
            // cout << "digit2 = "<<digit2 <<endl;

            // multiply each digit of num1 and num2
            int res = (digit1 * digit2 + carry) % base;
            carry = (digit1 * digit2 + carry) / base;

            // let the calculated res be inserted into atomic_prod
            prod.insert(0, 1, res + '0');

            // cout << "res = " << res <<  endl;
            // cout << "carry = " << carry << endl;
            // cout << "--------------"<<endl;
        }

        // after run out of digits to multiply -> we add the FINAL carry for
        prod.insert(0, 1, carry + '0');

        // pad 0(s) to the small product based on the i'th column (starts from leftmost)
        for (int k=0; k< num2.length() - 1 - i; k++){
            prod.push_back('0');
        }
        
        // add up the result of each small products
        product = add(product, prod, base);
        // cout << "product = " << product << endl;
        // cout << "----------------------------------"<<endl;
    }

    // remove leading 0s if any
    while ( !product.empty() && product.front() == '0' ){
        product.erase(0, 1);
    }

    // if the product is all 0 -> we might have deleted all of 0 -> now we add back one 0
    if ( product.empty() ){
        product.push_back('0');
    }

    return product;
}

void make_same_length(string& num1, string& num2){
    // make num1 and num2 have the same number of digits

    int pad_front = num1.length() - num2.length();

    // pad the front of num1 
    if ( pad_front < 0 ){
        num1.insert(0, -pad_front, '0');
    }
    // pad the front of num2
    else if ( pad_front > 0 ){
        num2.insert(0, pad_front, '0');
    }
}


string karatsuba(string num1, string num2, int base){

    int n = max(num1.length(), num2.length());

    // if there is no number at all -> return empty string 
    if ( n == 0 ){
        return string();
    }

    // use school method when there is less than 4 digits
    if ( n < 4 ){
        //cout << mult(num1, num2, base) <<endl;;
        return mult(num1, num2, base);
    }

    // recursive case -> use karatsuba
    
    // make num1 and num2 have same length
    make_same_length(num1, num2);

    // STEP 1: split the digits into 2 halves -> get a1, a0, b1, b0
    int k = n/2;
    string a1 = num1.substr(0, n-k);
    string a0 = num1.substr(n-k, k);
    string b1 = num2.substr(0, n-k);
    string b0 = num2.substr(n-k, k);

    // STEP 2: compute p0, p1, p2
    // p0 = (a1+a0) * (b1+b0), 
    // p1 = a1*b1, 
    // p2 = a0*b0
    
    string p2 = karatsuba(a1, b1, base);
    string p0 = karatsuba(a0, b0, base);

    string a1_add_a0 = add(a1, a0, base);
    string b1_add_b0 = add(b1, b0, base);
    string p1 = karatsuba(a1_add_a0, b1_add_b0, base);

    // STEP 3: combine p0, p1, p2 to make final final products
    // p = p2 * B^(2k) + (p1 - p2 - p0) * B^(k) + p0

    // cout << "before subtract p1"<<endl;
    p1 = subtract(p1, p2, base);
    p1 = subtract(p1, p0, base);

    // cout<< p1 <<endl;
    // cout << "after subtract p1 ---------------" <<endl;

    // pad 2*k numbers of "base" to p2
    // p2 = mult(p2, to_string(int(pow(base, 2*k))), base);
    p2.insert(p2.end(), 2*k, '0');
    // cout << "after pad, p2 = "<<p2 <<endl;

    // pad k numbers of "base" to p1
    p1.insert(p1.end(), k, '0');
    // p1 = mult(p1, to_string(int(pow(base, k))), base);
    // cout << "after pad, p1 = "<<p1 <<endl;

    // combine result
    string res = add(p2, p1, base);
    res = add(res, p0, base);

    return res;
}

string cleanup(string product){
    // remove leading 0s if any
    while ( !product.empty() && product.front() == '0' ){
        product.erase(0, 1);
    }

    // if the product is all 0 -> we might have deleted all of 0 -> now we add back one 0
    if ( product.empty() ){
        product.push_back('0');
    }

    return product;
}


int main(){

    string num1, num2;
    int base;

    cin >> num1 >> num2 >> base;

    cout << add(num1, num2, base) << " " << cleanup(karatsuba(num1, num2, base)) << " "<<0<<endl;

    if ( cleanup(karatsuba(num1, num2, base)) == "131310233310031001122333000231333100233333231232132213210122002022003021233"){
        cout << "true" << endl;
    }

    // TEST KARATSUBA
    // string test00 = "101";
    // string test01 = "5";
    // int test02 = 10;

    // string test10 = "10";
    // string test11 = "111";
    // int test12 = 2;

    // string test20 = "111";
    // string test21 = "10";
    // int test22 = 2;

    // string test10_0 = "12345";
    // string test10_1 = "67899";
    // int test10_2 = 10;

    // string test11_0 = "326";
    // string test11_1 = "98978";
    // int test11_2 = 10;

    // string test12_0 = "15";
    // string test12_1 = "2";
    // int test12_2 = 10;
    

    // cout << karatsuba(test00, test01, test02) <<endl;
    // cout << karatsuba(test10, test11, test12) <<endl;
    // cout << karatsuba(test20, test21, test22) <<endl;
    // cout << karatsuba(test10_0, test10_1, test10_2) <<endl;
    // cout << karatsuba(test11_0, test11_1, test11_2) <<endl;
    // cout << karatsuba(test12_0, test12_1, test12_2) <<endl;


    // TEST ADD
    // string test00 = "101";
    // string test01 = "5";
    // int test02 = 10;

    // string test10 = "10";
    // string test11 = "111";
    // int test12 = 2;

    // string test20 = "111";
    // string test21 = "10";
    // int test22 = 2;

    // string test30 = "123";
    // string test31 = "234";
    // int test32 = 6;

    // string test40 = "546";
    // string test41 = "248";
    // int test42 = 9;

    // cout << add(test00, test01, test02) <<endl;
    // cout << add(test10, test11, test12) <<endl;
    // cout << add(test20, test21, test22) <<endl;
    // cout << add(test30, test31, test32) <<endl;
    // cout << add(test40, test41, test42) <<endl;

    // TEST subtract (only works if num1 > num2, else hangs)
    // string test50 = "2321";
    // string test51 = "1332";
    // int test52 = 4;

    // string test60 = "111011";
    // string test61 = "10101";
    // int test62 = 2;

    // string test70 = "4213";
    // string test71 = "3522";
    // int test72 = 6;

    // cout << subtract(test50, test51, test52) << endl;
    // cout << subtract(test60, test61, test62) << endl;
    // cout << subtract(test70, test71, test72) << endl;


    // TEST MULT
    // string test00 = "101";
    // string test01 = "5";
    // int test02 = 10;

    // string test10 = "10";
    // string test11 = "111";
    // int test12 = 2;

    // string test20 = "111";
    // string test21 = "10";
    // int test22 = 2;

    // string test80 = "45";
    // string test81 = "34";
    // int test82 = 6;

    // string test90 = "32";
    // string test91 = "3";
    // int test92 = 4;

    // cout << mult(test00, test01, test02) <<endl;
    // cout << mult(test10, test11, test12) <<endl;
    // cout << mult(test20, test21, test22) <<endl;
    // cout << mult(test80, test81, test82) <<endl;
    // cout << mult(test90, test91, test92) <<endl;

   
    return 0;
}