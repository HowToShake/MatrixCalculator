#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <mutex>
#include <cstdlib>
#include <condition_variable>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

mutex mtx;
condition_variable cv;
int minValue = 0;

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

    int theBiggestValue = 0;
    int* theBiggestInRow;
    void setTheBiggestValue() {
        int localBiggest = theBiggestInRow[0];
        for (int i = 0; i < matrixesSize; i++) {
            if (theBiggestInRow[i] > localBiggest) {
                localBiggest = theBiggestInRow[i];
            }
        }
        this->theBiggestValue = localBiggest;
    }

    
    int theSmallestValue;
    int* theSmallestInRow;
    void setTheSmallestValue() {
        int localSmallest = theSmallestInRow[0];
        for (int i = 0; i < matrixesSize; i++) {
            if (theSmallestInRow[i] > localSmallest) {
                localSmallest = theSmallestInRow[i];
            }
        }
        this->theSmallestValue = localSmallest;
    }
   

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

       this->theBiggestInRow = new int[matrixesSize];
       this->theSmallestInRow = new int[matrixesSize];
       this->theSmallestValue = a.getElement(0, 0);
       
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


    void substract(int threadNumber) {

        mtx.lock();

        for (int i = threadNumber; i < threadNumber + 1; i++) {
            for (int j = 0; j < this->matrixesSize; j++) {

                result[i][j] = 0;
                result[i][j] = firstMatrix.getElement(i, j) - secondMatrix.getElement(i, j);

            }
        }

        mtx.unlock();

    }


    void findTheBiggestValueInRow(int threadNumber) {
        
        mtx.lock();
        int localBiggestValue = 0;
        for (int i = threadNumber; i < threadNumber + 1; i++) {
            for (int j = 0; j < this->matrixesSize; j++) {

                localBiggestValue = firstMatrix.getElement(i, j);
                
                
                if (localBiggestValue > theBiggestValue) {
                    this->theBiggestValue = localBiggestValue;
                }

            }
        }
        theBiggestInRow[threadNumber] = theBiggestValue;
        mtx.unlock();
    }


    void findTheSmallestValueInRow(int threadNumber) {
        mtx.lock();
        int localSmallestValue = firstMatrix.getElement(0,0);
        for (int i = threadNumber; i < threadNumber + 1; i++) {
            for (int j = 0; j < this->matrixesSize; j++) {

                localSmallestValue = firstMatrix.getElement(i, j);


                if (localSmallestValue < theSmallestValue) {
                    this->theSmallestValue = localSmallestValue;
                }

            }
        }
        theSmallestInRow[threadNumber] = theSmallestValue;
        mtx.unlock();
    }


    int getTheSmallestValue() {
        setTheSmallestValue();
        return theSmallestValue;
    }


    int getTheBiggestValue() {
        setTheBiggestValue();
        return theBiggestValue;
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

        int matrixExampleSize = 20;


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
        cout << "1.Matrix multiply." << endl;
        cout << "2.Matrix add." << endl;
        cout << "3.Matrix substract." << endl;
        cout << "4.The biggest value." << endl;
        cout << "5.The smallest value." << endl;

        int choice;
        cin >> choice;

        const int beginLoop = 0;
        vector <thread> threads;


        switch (choice) {
            case 1: {

                for (int i = beginLoop; i < matrixExampleSize; i++) {
                    thread t(&Calculator::multiply, calc, i);
                    threads.push_back(move(t));
                }

                for (auto& t : threads) {
                    t.join();
                }

                cout << "\n\RESULT MULTIPLY MATRIX: " << endl;
                calc.printResult();
                break;

            }

            case 2: {

                

                for (int i = beginLoop; i < matrixExampleSize; i++) {
                    thread t(&Calculator::add, calc, i);
                    threads.push_back(move(t));
                }

                for (auto& t : threads) {
                    t.join();
                }

                cout << "\n\RESULT ADD MATRIX: " << endl;
                calc.printResult();
                break;

            }

            case 3: {
                
                

                for (int i = beginLoop; i < matrixExampleSize; i++) {
                    thread t(&Calculator::substract, calc, i);
                    threads.push_back(move(t));
                }

                for (auto& t : threads) {
                    t.join();
                }

                cout << "\n\RESULT SUBSTRACT MATRIX: " << endl;
                calc.printResult();
                break;
            }

            case 4: {
                for (int i = beginLoop; i < matrixExampleSize; i++) {
                    thread t(&Calculator::findTheBiggestValueInRow, calc, i);
                    threads.push_back(move(t));
                }

                for (auto& t : threads) {
                    t.join();
                }

                cout << "\nTHE BIGGEST VALUE IN MATRIX: " << calc.getTheBiggestValue() <<endl;

                break;
            }

            case 5: {
                for (int i = beginLoop; i < matrixExampleSize; i++) {
                    thread t(&Calculator::findTheSmallestValueInRow, calc, i);
                    threads.push_back(move(t));
                }

                for (auto& t : threads) {
                    t.join();
                }

                cout << "\nTHE SMALLEST VALUE IN MATRIX: " << calc.getTheSmallestValue() << endl;

                break;
            }
            
        
          
        }


    }
};


int main()
{
    RenderWindow window(VideoMode(453, 453), "HEJKA");

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
            }
        }
    }

    Menu menu = Menu();

    menu.displayMenu();



    return 0;
   

}

