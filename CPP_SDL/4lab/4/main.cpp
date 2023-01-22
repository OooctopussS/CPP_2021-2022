#include <iostream>
#include <deque>
#include <locale>

using namespace std;

/*���� ���������� int � unsigned long.
1. ����� ������������ ��������� �������� �������.
2. ��� ���.*/


// ������� ������� ���� �����
void cleaning()
{
    cin.clear();
    while (cin.get() != '\n');
}

// ������ ������� ��� ����� ������� ��� ������ ������������ ��������� ��������
template <class T>
T* minOddEnter(T* a, const int &aSize){
    cout << "������� �������� ��������� �������: " << endl;
    for (int i = 0; i < aSize; i++){
        cout << "arr[" << i << "] = ";
        while(!(cin >> a[i])){
              cleaning();
              cout << endl << "������: ������� ��������� �������� ��������." << endl << endl;
              cout << "arr[" << i << "] = ";
        }
    }
        return a;
}

// ������ ������� ��� ������ ������������ ��������� ��������
template <class T>
T minOdd(T arr[], const int& size, T min){
    for(int i = 0; i < size; i++){
        if (arr[i] % 2 != 0){
            min = arr[i];
            break;
        }
    }
    for(int i = 0; i < size; i++){
        if(min > arr[i] && arr[i] % 2 != 0)
           min = arr[i];
    }
    return min;
}

// ������ ������� ��� ������ ���������� ������ ������������ �������� ��������
template <class T>
void minOddResult(T min){
    if (min % 2 == 0) cout << "����������� �������� ������� �� ������" << endl;
       else cout << "����������� �������� �������: " << min << endl;
}
template <class T>
void printTypeMenu(deque<T>* objDec){
    int stillWorkMenu = 1;
    int choiceMenu;
    do{
        system("CLS");
        cout << "1. ���������� �������� � ������." << endl;
        cout << "2. ���������� �������� � �����." << endl;
        cout << "3. �������� �������� �� ������." << endl;
        cout << "4. �������� �������� �� �����." << endl;
        cout << "5. �������� ������� �������� � ����." << endl;
        cout << "6. �������� ���������� �������� � ����." << endl;
        cout << "7. �������� ���." << endl;
        cout << "8. �����." << endl;
        cout << "�������� ����� ����: ";
        while (!(cin >> choiceMenu) || choiceMenu < 1  || choiceMenu > 8)
            {
                cleaning();
                cout << endl << "������: ������ �������������� ����� ����." << endl << endl;
                cout << "����� ����: ";
            }
        system("CLS");
        switch(choiceMenu){
            case 1:
            {
                T num;
                cout << "������� �������� ��� ��������: ";
                cin >> num;
                objDec->push_front(num);
                cout << "������� ������� �������� � ���." << endl;
                break;
            }
            case 2:
            {
                T num;
                cout << "������� �������� ��� ��������: ";
                cin >> num;
                objDec->push_back(num);
                cout << "������� ������� �������� � ���." << endl;
                break;
            }
            case 3:
                if(objDec->size())
                {
                   cout << "�������� ��������: " << objDec->front() << endl;
                   objDec->pop_front();
                } else cout << "�����..." << endl;
                break;
            case 4:
                if(objDec->size())
                {
                   cout << "�������� ��������: " << objDec->back() << endl;
                   objDec->pop_back();
                } else cout << "�����..." << endl;
                break;
            case 5:
                if(objDec->size())
                {
                    cout << "������ ������� � ����: " << objDec->front() << endl;
                } else cout << "�����..." << endl;
                break;
            case 6:
                if(objDec->size())
                {
                   cout << "��������� ������� � ����: " << objDec->back() << endl;
                } else cout << "�����..." << endl;
                break;
            case 7:
                if(objDec->size())
                {
                    objDec->clear();
                    cout << "��� �������� ���� ������� �������." << endl;
                } else cout << "����� ������� ������ ���???? -_- " << endl;
                break;
            case 8:
                stillWorkMenu = 0;
                break;
        }
        system("pause");
    }while(stillWorkMenu);
}

int main()
{
    setlocale(LC_ALL, "Rus");
    system("color 6");
    int stillWorkMenu = 1, choiceMenu, arrSize;
    do{
            system("CLS");
            cout << "\n1. ����� ������������ ��������� �������� �������." << endl;
            cout << "2. ��� ���." << endl;
            cout << "3. �����." << endl;
            cout << "\n������� ����� ����: ";
            while (!(cin >> choiceMenu) || choiceMenu < 1  || choiceMenu > 3)
                {
                    cleaning();
                    cout << endl << "������: ������ �������������� ����� ����." << endl << endl;
                    cout << "����� ����: ";
                }
            system("CLS");
            switch(choiceMenu){
                case 1:
                    cout << "������� ������ �������: ";
                    while (!(cin >> arrSize) || arrSize < 0)
                        {
                            cleaning();
                            cout << endl << "������: ����� ������������ ������ �������." << endl << endl;
                            cout << "������ �������: ";
                        }
                    cout << "1. ��� ��������� int." << endl;
                    cout << "2. ��� ��������� unsigned long." << endl;
                    cout << "�������� ��� ���������: ";
                    while (!(cin >> choiceMenu) || choiceMenu < 1  || choiceMenu > 2)
                        {
                            cleaning();
                            cout << endl << "������: ������ �������������� ����� ����." << endl << endl;
                            cout << "����� ����: ";
                        }
                    system("CLS");
                    switch(choiceMenu){
                        case 1:
                        {
                            int* array = new int[arrSize];
                            int min = 0;
                            array = minOddEnter(array, arrSize);
                            min = minOdd(array, arrSize, min);
                            minOddResult(min);
                            delete[] array;
                            break;
                        }
                        case 2:
                        {
                            unsigned long* array = new unsigned long[arrSize];
                            unsigned long min = 0;
                            array =  minOddEnter(array,arrSize);
                            min = minOdd(array, arrSize, min);
                            minOddResult(min);
                            delete[] array;
                            break;
                        }
                    }
                    break;
                case 2:
                cout << "1. ��� ��������� int." << endl;
                cout << "2. ��� ��������� unsigned long." << endl;
                cout << "�������� ��� ���������: ";
                while (!(cin >> choiceMenu) || choiceMenu < 1  || choiceMenu > 2)
                    {
                        cleaning();
                        cout << endl << "������: ������ �������������� ����� ����." << endl << endl;
                        cout << "����� ����: ";
                    }
                system("CLS");
                switch(choiceMenu){
                    case 1:
                    {
                        deque<int>* dec = new deque<int>();
                        printTypeMenu<int>(dec);
                        delete dec;
                        break;
                    }
                    case 2:
                        deque<unsigned long int>* dec = new deque<unsigned long int>();
                        printTypeMenu<unsigned long int>(dec);
                        delete dec;
                        break;
                }
                    break;
                case 3:
                    cout << "����������..." << endl;
                    stillWorkMenu = 0;
                    break;
            }
            system("pause");
    }while(stillWorkMenu);
    return 0;
}
