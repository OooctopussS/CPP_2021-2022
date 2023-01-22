#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <locale>
using namespace std;

/*Фирма по хранению и сбыту бытовых инструментов получает грузы с оборудованием по различным ценам и продает их затем с 20%-ной надбавкой, причем товары,
полученные ранее, продаются в первую очередь.
Напишите программу, считывающую записи о торговых операциях двух типов: операции по закупке и операции по продаже.
Запись о закупке содержит префикс “R”, количество товара и стоимость одного изделия.
После считывания записи о закупке напечатайте ее с указанием стоимости всей партии.
Запись о продаже содержит префикс “S” и количество товара.
После считывания записи об операции продажи напечатайте, сколько изделий было продано, цену одного изделия в каждой продаваемой партии, стоимость каждой партии и суммарную стоимость всей сделки.
Например, если фирмой были проданы 200 единиц оборудования, в которые входили 50 единиц с закупочной ценой 1.25$ и 150 единиц с закупочной ценой 1.1$, то напечатаны должны быть три строки:
50 штук по 1.50$ каждый на сумму 75.00$
150 штук по 1.32$ каждый на сумму 198.00$
Всего продано на сумму 273.00$
Если на складе отсутствует требуемое в заказе число изделий, то продайте все имеющиеся, а затем напечатайте сообщение об отсутствии остальной части изделий на складе.*/

// Тип данных
typedef struct tools{
    int type; // Тип операции (1 - операции по закупке, 2 - операция по продаже)
    int countProduct; // Количество товара
    double price; // Стоимость одного изделия
}DataType;

// Проверка
void cleaning()
{
    cin.clear();
    while (cin.get() != '\n');
}

// Добавление инструмента
DataType inputTool(){
    DataType tool;
    cout << "Тип операции (1 - операция по закупке 'R', 2 - операция по продаже 'S'): ";
    while (!(cin >> tool.type) || (tool.type != 1 && tool.type != 2)){
            cleaning();
            cout << endl << "Ошибка: несуществующий тип операции." << endl << endl;
            cout << "Тип операции: ";
        }
    cout << "Введите количество товара: ";
    while (!(cin >> tool.countProduct) || tool.countProduct < 0){
            cleaning();
            cout << endl << "Ошибка: некорректное количество товара." << endl << endl;
            cout << "Количество товара: ";
        }
    if (tool.type == 1){
        cout << "Введите стоимость одного изделия: ";
        while (!(cin >> tool.price) || tool.price < 0){
            cleaning();
            cout << endl << "Ошибка: некорректная цена." << endl << endl;
            cout << "Стоимость одного изделия: ";
        }
    }
    return tool;
}


// Абстрактный класс очереди
class Queue {
public:

    // Деструктор
    virtual ~Queue(){
    }

    virtual int EnQueue(DataType) = 0;
    virtual DataType* Front() = 0;
    virtual void DeQueue(DataType *) = 0;
    virtual int Full() = 0;
    virtual int Empty() = 0;
    virtual int nextEmpty() = 0;
};

// Реализация через список
class listQueue : public Queue{
private:
    struct element{
        DataType data;
        element* next;
    } * front, *rear; // Индексы головы и хвоста
public:
     listQueue(){
        front = rear = NULL;
    }

    // Добавление элемента в очередь
    int EnQueue(DataType x){
        element* temp = new (nothrow) element;
        if (temp == NULL)
            return 1;
        temp->data = x;
        temp->next = NULL;
        if (front == NULL)
            front = rear = temp;
        else
        {
            rear->next = temp;
            rear = rear->next;
        }
        return 1;
    }

    // Неразрушающее чтение элемента
    DataType* Front(void){
        return &front->data;
    }
    // Извлечение элемента из очереди
    void DeQueue(DataType *temp){
                front = front->next;
                delete temp;
    }

    // Полный список
    int Full(void){
        element* temp = new (nothrow) element;
        if (temp == NULL)
            return 1;
        delete temp;
        return 0;
    }
    // Пустой список
    int Empty(void){
        return front == NULL;
    }

    // Проверка следующего элемента
    int nextEmpty(void){
        return front->next == nullptr;
    }

     // Деструктор
    ~listQueue(){
        element* temp = front;
        while (front)
        {
            temp = front;
            front = front->next;
            delete temp;
        }
    }
};

// Массив
class masQueue : public Queue{
private:
    int front, rear; // Индексы головы и хвоста
    int maxlength; // Размер массива
    DataType* data; // Массив для хранения элементов
public:
    // Конструктор
    masQueue(int n = 10): maxlength(n){
        data = new DataType[maxlength];
        front = 0;
        rear = maxlength - 1;
    }

    // Деструктор
    ~masQueue(){
        delete[] data;
    }

    // Проверка на пустоту
    int Empty(void){
        return (rear + 1) % maxlength == front;
    }

    // Проверка следующего элемента
    int nextEmpty(void){
        return front == rear;
    }

    // Проверка на полноту заполнения
    int Full(void){
        return (rear + 2) % maxlength == front;
    }

    // неразрушающее чтение элемента
    DataType* Front(void){
        return &data[front];
    }

    // Добавление элемента в очередь
    int EnQueue(DataType x){
        if (Full())
            return 0;
        else{
        rear = (rear + 1) % maxlength;
        data[rear] = x;
        return 1;
        }
    }

    // Извлечение элемента из очереди
    void DeQueue(DataType * temp){
            front = (front + 1) % maxlength;
    }
};

// Вывод товара с типом операции
void printDataType(DataType tool){
    cout << endl << "1. Тип операции: ";
   (tool.type == 1) ? cout << "R" << endl : cout << "S" << endl;
    cout << "2. Количество товаров: " << tool.countProduct << endl;
    cout << "3. Стоимость одного изделия: " << fixed << setprecision(2) << tool.price << endl;
}

// Вывод меню
void printMenu(Queue* objQueue){
    DataType tool;
    int choice;
    int stillWork = 1;
    double res;
    int percent = 20;
    do{
            system("CLS");
            cout << endl << "1. Торговая операция." << endl;
            cout << "2. Изменить процент надбавки." << endl;
            cout << "0. Выход" << endl;
            cout << "\nВведите пункт меню: ";
           // Проверка
            while (!(cin >> choice) || choice < 0  || choice > 2)
                {
                    cleaning();
                    cout << endl << "Ошибка: выбран несуществующий пункт меню." << endl << endl;
                    cout << "Пункт меню: ";
                }
            system("CLS");
            switch(choice){
                case 0:
                    stillWork = 0;
                    break;
                case 1:
                tool = inputTool();
                if (tool.type == 1){
                    if(!objQueue->Full()){
                    objQueue->EnQueue(tool);
                    printDataType(tool);
                    cout << endl << "Стоимость всей партии: " <<  fixed << setprecision(2) << tool.price * tool.countProduct << endl;
                    } else cout << "Список полон" << endl;
                } else if (tool.type == 2){
                    //Если не пуст
                    if(!objQueue->Empty()){
                    res = 0;
                    // Вычисляем прибыль с процентами
                    double percentProfit = 1 + percent / (double)100;
                    DataType* temp = objQueue->Front();
                    // Проверка на то, что продано меньше, чем есть на складе одного товара
                    if (temp->countProduct - tool.countProduct > 0 && objQueue->Front()){
                        temp->countProduct -= tool.countProduct;
                        cout << "Продано товаров: " << tool.countProduct << endl;
                        cout << "По: " << temp->price * percentProfit << endl;
                        res = tool.countProduct * temp->price * percentProfit;
                    } else{
                        while(tool.countProduct > 0){
                            // Проверка на то, что на складе меньше товара, чем требуется продать
                            if (objQueue->nextEmpty() && temp->countProduct - tool.countProduct < 0){
                                cout << "Было продано: " << temp->countProduct << endl << "Остальная часть на складке отсутсвует"  << endl;
                                tool.countProduct = 0;
                            }
                            else{
                                cout << "Проданов товаров: " << temp->countProduct << endl;
                                tool.countProduct -= temp->countProduct;
                                }
                            cout << "По: " << temp->price * percentProfit << endl;
                            res += temp->countProduct * temp->price * percentProfit;
                            objQueue->DeQueue(temp);
                            temp = objQueue->Front();
                        }
                    }
                    cout << endl << "Всего проданно на сумму: " << fixed << setprecision(2) << res << endl;
                    } else cout << "Список пуст" << endl;
                }
                    break;
                case 2:
                    cout << "Текущий процент надбавки: " << percent << "%" << endl;
                    cout << "Введите новый процент надбавки: " << endl;
                    while (!(cin >> percent) || percent < 1)
                        {
                            cleaning();
                            cout << endl << "Ошибка: выбрано неверное значение." << endl << endl;
                            cout << "Процент надбавки: ";
                        }
                    cout << "Процент успешно изменен." << endl;
                    break;
                default: cout << "Некорректный ввод";
                }
            system("pause");
        }while(stillWork);
}

int main(void){
    system("color 6");
    setlocale(LC_ALL, "Rus");

    int structureChoice;
    system("CLS");
    cout << "1. Векторная структура хранения." << endl;
    cout << "2. Связная структура хранения." << endl;
    cout << "Выберите структуру хранения: ";
    while (!(cin >> structureChoice) || structureChoice < 1  || structureChoice > 2)
        {
            cleaning();
            cout << endl << "Ошибка: выбран несуществующий пункт. " << endl << endl;
            cout << "Пункт меню: ";
        }
    if (structureChoice == 1){
        masQueue* q = new masQueue;
        Queue* object = &(*q);
        printMenu(object);
        delete object;
    }
    else if (structureChoice == 2){
        listQueue* q = new listQueue;
        Queue* object = &(*q);
        printMenu(object);
        delete object;
    }
    return 0;
}
