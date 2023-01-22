#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <locale>
using namespace std;

/*����� �� �������� � ����� ������� ������������ �������� ����� � ������������� �� ��������� ����� � ������� �� ����� � 20%-��� ���������, ������ ������,
���������� �����, ��������� � ������ �������.
�������� ���������, ����������� ������ � �������� ��������� ���� �����: �������� �� ������� � �������� �� �������.
������ � ������� �������� ������� �R�, ���������� ������ � ��������� ������ �������.
����� ���������� ������ � ������� ����������� �� � ��������� ��������� ���� ������.
������ � ������� �������� ������� �S� � ���������� ������.
����� ���������� ������ �� �������� ������� �����������, ������� ������� ���� �������, ���� ������ ������� � ������ ����������� ������, ��������� ������ ������ � ��������� ��������� ���� ������.
��������, ���� ������ ���� ������� 200 ������ ������������, � ������� ������� 50 ������ � ���������� ����� 1.25$ � 150 ������ � ���������� ����� 1.1$, �� ���������� ������ ���� ��� ������:
50 ���� �� 1.50$ ������ �� ����� 75.00$
150 ���� �� 1.32$ ������ �� ����� 198.00$
����� ������� �� ����� 273.00$
���� �� ������ ����������� ��������� � ������ ����� �������, �� �������� ��� ���������, � ����� ����������� ��������� �� ���������� ��������� ����� ������� �� ������.*/

// ��� ������
typedef struct tools{
    int type; // ��� �������� (1 - �������� �� �������, 2 - �������� �� �������)
    int countProduct; // ���������� ������
    double price; // ��������� ������ �������
}DataType;

// ��������
void cleaning()
{
    cin.clear();
    while (cin.get() != '\n');
}

// ���������� �����������
DataType inputTool(){
    DataType tool;
    cout << "��� �������� (1 - �������� �� ������� 'R', 2 - �������� �� ������� 'S'): ";
    while (!(cin >> tool.type) || (tool.type != 1 && tool.type != 2)){
            cleaning();
            cout << endl << "������: �������������� ��� ��������." << endl << endl;
            cout << "��� ��������: ";
        }
    cout << "������� ���������� ������: ";
    while (!(cin >> tool.countProduct) || tool.countProduct < 0){
            cleaning();
            cout << endl << "������: ������������ ���������� ������." << endl << endl;
            cout << "���������� ������: ";
        }
    if (tool.type == 1){
        cout << "������� ��������� ������ �������: ";
        while (!(cin >> tool.price) || tool.price < 0){
            cleaning();
            cout << endl << "������: ������������ ����." << endl << endl;
            cout << "��������� ������ �������: ";
        }
    }
    return tool;
}


// ����������� ����� �������
class Queue {
public:

    // ����������
    virtual ~Queue(){
    }

    virtual int EnQueue(DataType) = 0;
    virtual DataType* Front() = 0;
    virtual void DeQueue(DataType *) = 0;
    virtual int Full() = 0;
    virtual int Empty() = 0;
    virtual int nextEmpty() = 0;
};

// ���������� ����� ������
class listQueue : public Queue{
private:
    struct element{
        DataType data;
        element* next;
    } * front, *rear; // ������� ������ � ������
public:
     listQueue(){
        front = rear = NULL;
    }

    // ���������� �������� � �������
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

    // ������������� ������ ��������
    DataType* Front(void){
        return &front->data;
    }
    // ���������� �������� �� �������
    void DeQueue(DataType *temp){
                front = front->next;
                delete temp;
    }

    // ������ ������
    int Full(void){
        element* temp = new (nothrow) element;
        if (temp == NULL)
            return 1;
        delete temp;
        return 0;
    }
    // ������ ������
    int Empty(void){
        return front == NULL;
    }

    // �������� ���������� ��������
    int nextEmpty(void){
        return front->next == nullptr;
    }

     // ����������
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

// ������
class masQueue : public Queue{
private:
    int front, rear; // ������� ������ � ������
    int maxlength; // ������ �������
    DataType* data; // ������ ��� �������� ���������
public:
    // �����������
    masQueue(int n = 10): maxlength(n){
        data = new DataType[maxlength];
        front = 0;
        rear = maxlength - 1;
    }

    // ����������
    ~masQueue(){
        delete[] data;
    }

    // �������� �� �������
    int Empty(void){
        return (rear + 1) % maxlength == front;
    }

    // �������� ���������� ��������
    int nextEmpty(void){
        return front == rear;
    }

    // �������� �� ������� ����������
    int Full(void){
        return (rear + 2) % maxlength == front;
    }

    // ������������� ������ ��������
    DataType* Front(void){
        return &data[front];
    }

    // ���������� �������� � �������
    int EnQueue(DataType x){
        if (Full())
            return 0;
        else{
        rear = (rear + 1) % maxlength;
        data[rear] = x;
        return 1;
        }
    }

    // ���������� �������� �� �������
    void DeQueue(DataType * temp){
            front = (front + 1) % maxlength;
    }
};

// ����� ������ � ����� ��������
void printDataType(DataType tool){
    cout << endl << "1. ��� ��������: ";
   (tool.type == 1) ? cout << "R" << endl : cout << "S" << endl;
    cout << "2. ���������� �������: " << tool.countProduct << endl;
    cout << "3. ��������� ������ �������: " << fixed << setprecision(2) << tool.price << endl;
}

// ����� ����
void printMenu(Queue* objQueue){
    DataType tool;
    int choice;
    int stillWork = 1;
    double res;
    int percent = 20;
    do{
            system("CLS");
            cout << endl << "1. �������� ��������." << endl;
            cout << "2. �������� ������� ��������." << endl;
            cout << "0. �����" << endl;
            cout << "\n������� ����� ����: ";
           // ��������
            while (!(cin >> choice) || choice < 0  || choice > 2)
                {
                    cleaning();
                    cout << endl << "������: ������ �������������� ����� ����." << endl << endl;
                    cout << "����� ����: ";
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
                    cout << endl << "��������� ���� ������: " <<  fixed << setprecision(2) << tool.price * tool.countProduct << endl;
                    } else cout << "������ �����" << endl;
                } else if (tool.type == 2){
                    //���� �� ����
                    if(!objQueue->Empty()){
                    res = 0;
                    // ��������� ������� � ����������
                    double percentProfit = 1 + percent / (double)100;
                    DataType* temp = objQueue->Front();
                    // �������� �� ��, ��� ������� ������, ��� ���� �� ������ ������ ������
                    if (temp->countProduct - tool.countProduct > 0 && objQueue->Front()){
                        temp->countProduct -= tool.countProduct;
                        cout << "������� �������: " << tool.countProduct << endl;
                        cout << "��: " << temp->price * percentProfit << endl;
                        res = tool.countProduct * temp->price * percentProfit;
                    } else{
                        while(tool.countProduct > 0){
                            // �������� �� ��, ��� �� ������ ������ ������, ��� ��������� �������
                            if (objQueue->nextEmpty() && temp->countProduct - tool.countProduct < 0){
                                cout << "���� �������: " << temp->countProduct << endl << "��������� ����� �� ������� ����������"  << endl;
                                tool.countProduct = 0;
                            }
                            else{
                                cout << "�������� �������: " << temp->countProduct << endl;
                                tool.countProduct -= temp->countProduct;
                                }
                            cout << "��: " << temp->price * percentProfit << endl;
                            res += temp->countProduct * temp->price * percentProfit;
                            objQueue->DeQueue(temp);
                            temp = objQueue->Front();
                        }
                    }
                    cout << endl << "����� �������� �� �����: " << fixed << setprecision(2) << res << endl;
                    } else cout << "������ ����" << endl;
                }
                    break;
                case 2:
                    cout << "������� ������� ��������: " << percent << "%" << endl;
                    cout << "������� ����� ������� ��������: " << endl;
                    while (!(cin >> percent) || percent < 1)
                        {
                            cleaning();
                            cout << endl << "������: ������� �������� ��������." << endl << endl;
                            cout << "������� ��������: ";
                        }
                    cout << "������� ������� �������." << endl;
                    break;
                default: cout << "������������ ����";
                }
            system("pause");
        }while(stillWork);
}

int main(void){
    system("color 6");
    setlocale(LC_ALL, "Rus");

    int structureChoice;
    system("CLS");
    cout << "1. ��������� ��������� ��������." << endl;
    cout << "2. ������� ��������� ��������." << endl;
    cout << "�������� ��������� ��������: ";
    while (!(cin >> structureChoice) || structureChoice < 1  || structureChoice > 2)
        {
            cleaning();
            cout << endl << "������: ������ �������������� �����. " << endl << endl;
            cout << "����� ����: ";
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
