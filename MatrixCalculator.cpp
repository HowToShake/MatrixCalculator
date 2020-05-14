#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <mutex>
#include <cstdlib>

using namespace std;

mutex mtx;

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


    void multiply(int threadNumber){

        mtx.lock();
        
        for (int i = threadNumber; i < threadNumber+1; i++) {
            for (int j = 0; j < this->matrixesSize; j++) {
                result[i][j] = 0;
                for (int k = 0; k < this->matrixesSize; k++) {
                    result[i][j] += firstMatrix.getElement(i, k)*secondMatrix.getElement(k, j);
                }
            }
        }


        mtx.unlock();
    }


    void add(int threadNumber) {

        mtx.lock();

        for (int i = threadNumber; i < threadNumber + 1; i++) {
            for (int j = 0; j < this->matrixesSize; j++) {

                result[i][j] = 0;
                result[i][j] = firstMatrix.getElement(i, j) + secondMatrix.getElement(i, j);
                
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

class Menu {

public:



    void displayMenu() {

        int matrixExampleSize = 3;


        Matrix A = Matrix(matrixExampleSize);
        A.setRandomElements();
        cout << "FIRST MATRIX: " << endl;
        A.printMatrix();

        Matrix B = Matrix(matrixExampleSize);
        B.setRandomElements();
        cout << "\n\nSECOND MATRIX: " << endl;
        B.printMatrix();


        Calculator calc = Calculator(A, B);
        

        cout << "\nChoose an option: " << endl;
        cout << "1.Matrix multiply.\n" << endl;
        cout << "2.Matrix add.\n" << endl;

        int choice;
        cin >> choice;

        const int beginLoop = 0;

        switch (choice) {
            case 1: {

                vector <thread> threads;

                for (int i = beginLoop; i < matrixExampleSize; i++) {
                    thread t(&Calculator::multiply, calc, i);
                    threads.push_back(move(t));
                }

                for (auto& t : threads) {
                    t.join();
                }

                cout << "\n\RESULT MULTIPLY MATRIX: " << endl;
                calc.printResult();

            }
            case 2: {

                vector <thread> threads;

                for (int i = beginLoop; i < matrixExampleSize; i++) {
                    thread t(&Calculator::add, calc, i);
                    threads.push_back(move(t));
                }

                for (auto& t : threads) {
                    t.join();
                }

                cout << "\n\RESULT ADD MATRIX: " << endl;
                calc.printResult();
                

            }

        }


    }
};


int main()
{
    
    Menu menu = Menu();

    menu.displayMenu();
    

    return 0;
   

}

