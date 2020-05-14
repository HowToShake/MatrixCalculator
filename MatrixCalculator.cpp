#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <mutex>
#include <cstdlib>

using namespace std;

class Matrix {
private: 
     int matrixSize;
     int** elements;
     int maxValueToRandGenerator = 5;
public:
    
    Matrix() {

        this->matrixSize = 10;
    }

    Matrix(int size) {
        this->matrixSize = size;
        int** elements = new int* [size];
        for (int i = 0; i < size; i++) {
            elements[i] = new int[size];
        }
        this->elements = elements;
    }

    void setRandomElements() {
        random_device device;
        mt19937 generator(device());
        uniform_int_distribution<int> dist(1, this->maxValueToRandGenerator);

        for (int i = 0; i < this->matrixSize; i++) {
            for (int j = 0; j < this->matrixSize; j++) {
                int a = int(dist(generator));
                elements[i][j] = a;
            }
        }
    }

    void printMatrix() {
        for (int i = 0; i < this->matrixSize; i++) {
            for (int j = 0; j < this->matrixSize; j++) {
                cout << this->elements[i][j] << " ";
            }
            cout << "\n";
        }
    }

    int getElement(int row, int column) {

        if ((row >= this->matrixSize)||(column >= this->matrixSize)) {
            cout << "Wyszedles poza zakres macierzy." << endl;
            return 0;
        }

        return this->elements[row][column];
    }

    int getMatrixSize() {
        return this->matrixSize;
    }


};

mutex mtx;


class Calculator {
private:
    Matrix firstMatrix, secondMatrix;
    int matrixesSize;
    int** result;

public:
    

    Calculator(Matrix a, Matrix b){
       int bufor = a.getMatrixSize();
       this->firstMatrix = a;
       this->secondMatrix =  b;
       this->matrixesSize = bufor;

       int** result = new int* [bufor];

       for (int i = 0; i < bufor; i++) {
           result[i] = new int[bufor];
       }

       this->result = result;
       
    }


    void multiply(){

        mtx.lock();
        
        for (int i = 0; i < this->matrixesSize; i++) {
            for (int j = 0; j < this->matrixesSize; j++) {
                result[i][j] = 0;
                for (int k = 0; k < this->matrixesSize; k++) {
                    result[i][j] += firstMatrix.getElement(i, k)*secondMatrix.getElement(k, j);
                }
            }
        }


        mtx.unlock();
    }

    
    void printResult() {
        for (int i = 0; i < this->matrixesSize; i++) {
            for (int j = 0; j < this->matrixesSize; j++) {
                cout << result[i][j] << " ";
            }
            cout << "\n";
        }
    }


    void printMatrix(int matrix) {
        Matrix displayed;

        if (matrix == 1) {
            displayed = this->firstMatrix;
        }
        else if(matrix == 2) {
            displayed = this->secondMatrix;
        }
        else {
            cout << "There is no matrix." << endl;
        }


        for (int i = 0; i < this->matrixesSize; i++) {
            for (int j = 0; j < this->matrixesSize; j++) {
                cout << displayed.getElement(i, j)<<" ";
            }
            cout << "\n";
        }
    }

};


int main()
{
    Matrix A = Matrix(4);
    A.setRandomElements();
    cout << "FIRST MATRIX: " << endl;
    A.printMatrix();

    Matrix B = Matrix(4);
    B.setRandomElements();
    cout << "\n\nSECOND MATRIX: " << endl;
    B.printMatrix();


    Calculator calc = Calculator(A, B);

    thread t1(&Calculator::multiply, calc);
    thread t2(&Calculator::multiply, calc);

    t1.join();
    //t2.join();
    int waiter;
    cin >> waiter;
    calc.printResult();

    return 0;
   

}

