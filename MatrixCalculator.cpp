#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <mutex>
#include <cstdlib>
#include <condition_variable>
#include <sstream>
#include <vector>
#include <SFML/Graphics.hpp>


using namespace std;


mutex mtx;
condition_variable cv;
int maxValueSecondMatrix = 0;
int minValueSecondMatrix = INT_MAX;
int counter;



class Matrix {
private: 
     int matrixSize;
     int** elements;
     int maxValueToRandGenerator = 1000;
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

    string getMatrixValues() {
        string temporary = "";
        for (int i = 0; i < this->matrixSize; i++) {
            for (int j = 0; j < this->matrixSize; j++) {
                
                if (this->elements[i][j] < 10) {
                    temporary += to_string(this->elements[i][j]);
                    temporary += "        ";
                }
                else if (this->elements[i][j] >= 10 && this->elements[i][j] <= 99) {
                    temporary += to_string(this->elements[i][j]);
                    temporary += "      ";
                }
                else if (this->elements[i][j] >= 100 && this->elements[i][j] <= 999) {
                    temporary += to_string(this->elements[i][j]);
                    temporary += "    ";
                }
                else {
                    temporary += to_string(this->elements[i][j]);
                    temporary += "  ";
                }
                                
            }
            temporary += "\n";
        }
        return temporary;
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
            if (theSmallestInRow[i] < localSmallest) {
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
       counter = bufor * bufor;
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


    void getTheBiggestValueForSecondMatrix() {
        unique_lock<mutex> ul(mtx);
        cv.wait(ul, [] {return (maxValueSecondMatrix != 0 && counter == 0) ? true : false; });
        //cout << "MAX VALUE IN SECOND MATRIX IS: " << maxValueSecondMatrix << endl;
    
    }


    void findTheBiggestValueForSecondMatrix(int threadNumber) {
        lock_guard<mutex> lg(mtx);
        for (int i = threadNumber; i < threadNumber + 1; i++) {
            for (int j = 0; j < matrixesSize; j++) {
                counter--;
                if (secondMatrix.getElement(i, j) > maxValueSecondMatrix) {
                    maxValueSecondMatrix = secondMatrix.getElement(i, j);
                }
            }
        }
        cv.notify_all();
    }


    void getTheSmallestValueForSecondMatrix() {
        unique_lock<mutex> ul(mtx);
        cv.wait(ul, [] {return (minValueSecondMatrix != INT_MAX && counter == 0 ) ? true : false; });
        //cout << "MIN VALUE IN SECOND MATRIX IS: " << minValueSecondMatrix << endl;
    }


    void findTheSmallestValueForSecondMatrix(int threadNumber) {
        lock_guard<mutex> lg(mtx);
        for (int i = threadNumber; i < threadNumber + 1; i++) {
            for (int j = 0; j < matrixesSize; j++) {
                counter--;
                if (secondMatrix.getElement(i, j) < minValueSecondMatrix) {
                    minValueSecondMatrix = secondMatrix.getElement(i, j);
                }
            }
        }
        cv.notify_all();
    }


    void printResult() {
        for (int i = 0; i < this->matrixesSize; i++) {
            for (int j = 0; j < this->matrixesSize; j++) {
                cout << result[i][j] << " ";
            }
            cout << "\n";
        }
    }


    string getResult() {
        string temporary = "";
        for (int i = 0; i < this->matrixesSize; i++) {
            for (int j = 0; j < this->matrixesSize; j++) {

                if (this->result[i][j] >= 0 && this->result[i][j] < 10){
                    temporary += to_string(this->result[i][j]);
                    temporary += "        ";
                }
                else if (this->result[i][j] >= 10 && this->result[i][j] <= 99){
                    temporary += to_string(this->result[i][j]);
                    temporary += "      ";
                }
                else if (this->result[i][j] >= 100 && this->result[i][j] <= 999){
                    temporary += to_string(this->result[i][j]);
                    temporary += "    ";
                }
                else if (this->result[i][j] <= -1 && this->result[i][j] >= -9) {
                    temporary += to_string(this->result[i][j]);
                    temporary += "       ";
                }
                else if (this->result[i][j] <= -10 && this->result[i][j] >= -99) {
                    temporary += to_string(this->result[i][j]);
                    temporary += "     ";
                }
                else if (this->result[i][j] <= -100 && this->result[i][j] >= -999) {
                    temporary += to_string(this->result[i][j]);
                    temporary += "   ";
                }
                else if (this->result[i][j] <= -1000 && this->result[i][j] >= -9999) {
                    temporary += to_string(this->result[i][j]);
                    temporary += " ";
                }
                else {
                    temporary += to_string(this->result[i][j]);
                    temporary += "  ";
                }

            }
            temporary += "\n";
        }
        return temporary;
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
        cout << "6.The biggest value for second matrix." << endl;
        cout << "7.The smallest value for second matrix." << endl;

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

                cout << "\nRESULT MULTIPLY MATRIX: " << endl;
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

                cout << "\nRESULT ADD MATRIX: " << endl;
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

                cout << "\nRESULT SUBSTRACT MATRIX: " << endl;
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

                //cout << "\nTHE BIGGEST VALUE IN MATRIX: " << calc.getTheBiggestValue() <<endl;

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

                //cout << "\nTHE SMALLEST VALUE IN MATRIX: " << calc.getTheSmallestValue() << endl;

                break;
            }
            
            case 6: {
                for (int i = beginLoop; i < matrixExampleSize; i++) {
                    thread t(&Calculator::findTheBiggestValueForSecondMatrix, calc, i);
                    threads.push_back(move(t));
                }
                thread tEnd(&Calculator::getTheBiggestValueForSecondMatrix, calc);
                threads.push_back(move(tEnd));
                for (auto& t : threads) {
                    t.join();
                }

                break;
                

            }
        
            case 7: {
                for (int i = beginLoop; i < matrixExampleSize; i++) {
                    thread t(&Calculator::findTheSmallestValueForSecondMatrix, calc, i);
                    threads.push_back(move(t));
                }
                thread tEnd(&Calculator::getTheSmallestValueForSecondMatrix, calc);
                threads.push_back(move(tEnd));
                for (auto& t : threads) {
                    t.join();
                }
                break;
            }
        }


    }
};


int main()
{
    int matrixSize = 14;
    Matrix matrixOne = Matrix(matrixSize);
    matrixOne.setRandomElements();
    
    
    Matrix matrixTwo = Matrix(matrixSize);
    matrixTwo.setRandomElements();

    string menuString = "Choose an option : \n0.Reset.\n1.Matrix multiply.\n2.Matrix add.\n3.Matrix substract.\n4.The biggest value.\n5.The smallest value.\n6.The biggest value for second matrix.\n7.The smallest value for second matrix.\n";
    

    sf::RenderWindow window(sf::VideoMode(2120, 1080), "Matrix Calculator");
    
    sf::Text text;
    sf::Text secondText;
    sf::Text firstTitle;
    sf::Text secondTitle;
    sf::Text result;
    sf::Text menu;

    sf::Font font;
    font.loadFromFile("arial.ttf");

    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    secondText.setFont(font);
    secondText.setCharacterSize(24);
    secondText.setFillColor(sf::Color::White);

    firstTitle.setFont(font);
    firstTitle.setCharacterSize(24);
    firstTitle.setFillColor(sf::Color::White);

    result.setFont(font);
    result.setCharacterSize(24);
    result.setFillColor(sf::Color::White);

    firstTitle.setFont(font);
    firstTitle.setCharacterSize(48);
    firstTitle.setFillColor(sf::Color::White);

    secondTitle.setFont(font);
    secondTitle.setCharacterSize(48);
    secondTitle.setFillColor(sf::Color::White);

    menu.setFont(font);
    menu.setCharacterSize(24);
    menu.setFillColor(sf::Color::White);


    text.setString(matrixOne.getMatrixValues());
    secondText.setString(matrixTwo.getMatrixValues());
    firstTitle.setString("First matrix");
    secondTitle.setString("Second matrix");
    result.setString("RESULT");
    menu.setString(menuString);


    const int beginLoop = 0;
    vector <thread> threads;
    int operation = 1;

    

    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();


            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && (sf::Mouse::getPosition(window).x <= 950 && sf::Mouse::getPosition(window).x >= 100) && (sf::Mouse::getPosition(window).y <= 500 && sf::Mouse::getPosition(window).y >= 50)) {

                matrixOne.setRandomElements();
                text.setString(matrixOne.getMatrixValues());
                operation = 1;
                threads.clear();
                maxValueSecondMatrix = 0;
                minValueSecondMatrix = INT_MAX;
                counter = matrixSize * matrixSize;

            }


            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && (sf::Mouse::getPosition(window).x <= 1900 && sf::Mouse::getPosition(window).x >= 1100) && (sf::Mouse::getPosition(window).y <= 500 && sf::Mouse::getPosition(window).y >= 50)) {

                matrixTwo.setRandomElements();
                secondText.setString(matrixTwo.getMatrixValues());
                operation = 1;
                threads.clear();
                maxValueSecondMatrix = 0;
                minValueSecondMatrix = INT_MAX;
                counter = matrixSize * matrixSize;

            }


            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) && operation != 0) {
                operation--;
                Calculator calculator = Calculator(matrixOne, matrixTwo);
                
                for (int i = beginLoop; i < matrixSize; i++) {
                    thread t(&Calculator::multiply, calculator, i);
                    threads.push_back(move(t));
                }
                
                for (auto& t : threads) {
                    t.join();
                }
                
                string temporary = calculator.getResult();

                result.setString(temporary);
            }


            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) && operation != 0) {
                operation--;
                Calculator calculator = Calculator(matrixOne, matrixTwo);

                for (int i = beginLoop; i < matrixSize; i++) {
                    thread t(&Calculator::add, calculator, i);
                    threads.push_back(move(t));
                }

                for (auto& t : threads) {
                    t.join();
                }

                string temporary = calculator.getResult();

                result.setString(temporary);
            }


            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) && operation != 0) {
                operation--;
                Calculator calculator = Calculator(matrixOne, matrixTwo);

                for (int i = beginLoop; i < matrixSize; i++) {
                    thread t(&Calculator::substract, calculator, i);
                    threads.push_back(move(t));
                }

                for (auto& t : threads) {
                    t.join();
                }

                string temporary = calculator.getResult();

                result.setString(temporary);
            }


            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4) && operation != 0) {
                operation--;
                Calculator calculator = Calculator(matrixOne, matrixTwo);

                for (int i = beginLoop; i < matrixSize; i++) {
                    thread t(&Calculator::findTheBiggestValueInRow, calculator, i);
                    threads.push_back(move(t));
                }

                for (auto& t : threads) {
                    t.join();
                }

                string temporary = to_string(calculator.getTheBiggestValue());

                result.setString(temporary);
            }


            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5) && operation != 0) {
                operation--;
                Calculator calculator = Calculator(matrixOne, matrixTwo);

                for (int i = beginLoop; i < matrixSize; i++) {
                    thread t(&Calculator::findTheSmallestValueInRow, calculator, i);
                    threads.push_back(move(t));
                }

                for (auto& t : threads) {
                    t.join();
                }

                string temporary = to_string(calculator.getTheSmallestValue());

                result.setString(temporary);
            }


            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6) && operation != 0) {
                operation--;
                Calculator calculator = Calculator(matrixOne, matrixTwo);

                for (int i = beginLoop; i < matrixSize; i++) {
                    thread t(&Calculator::findTheBiggestValueForSecondMatrix, calculator, i);
                    threads.push_back(move(t));
                }

                thread tEnd(&Calculator::getTheBiggestValueForSecondMatrix, calculator);
                threads.push_back(move(tEnd));
                for (auto& t : threads) {
                    t.join();
                }

                string temporary = to_string(maxValueSecondMatrix);

                result.setString(temporary);
            }


            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7) && operation != 0) {
                operation--;
                Calculator calculator = Calculator(matrixOne, matrixTwo);

                for (int i = beginLoop; i < matrixSize; i++) {
                    thread t(&Calculator::findTheSmallestValueForSecondMatrix, calculator, i);
                    threads.push_back(move(t));
                }
                thread tEnd(&Calculator::getTheSmallestValueForSecondMatrix, calculator);
                threads.push_back(move(tEnd));
                for (auto& t : threads) {
                    t.join();
                }

                string temporary = to_string(minValueSecondMatrix);

                result.setString(temporary);
            }


            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0) && operation == 0) {
                operation = 1;
                threads.clear();
                maxValueSecondMatrix = 0;
                minValueSecondMatrix = INT_MAX;
                counter = matrixSize * matrixSize;
            }


        }


      


        text.setPosition(100, 100);
        secondText.setPosition(1100, 100);
        firstTitle.setPosition(100, 30);
        secondTitle.setPosition(1100, 30);

        result.setPosition(600, 550);
        menu.setPosition(100, 550);

        window.clear();


        window.draw(text);
        window.draw(secondText);
        window.draw(firstTitle);
        window.draw(secondTitle);
        window.draw(result);
        window.draw(menu);


        window.display();
    }
    


    //Menu menu = Menu();
    //menu.displayMenu();
    return 0;
}
