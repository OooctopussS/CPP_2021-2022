#include <iostream>
#include <deque>
#include <locale>

using namespace std;

/*Типы аргументов int и unsigned long.
1. Поиск минимального нечетного элемента массива.
2. АТД Дек.*/


// Функция очистки поля ввода
void cleaning()
{
    cin.clear();
    while (cin.get() != '\n');
}

// Шаблон функции для ввода массива для поиска минимального нечетного элемента
template <class T>
T* minOddEnter(T* a, const int &aSize){
    cout << "Введите значение элементов массива: " << endl;
    for (int i = 0; i < aSize; i++){
        cout << "arr[" << i << "] = ";
        while(!(cin >> a[i])){
              cleaning();
              cout << endl << "Ошибка: введите нормально значение элемента." << endl << endl;
              cout << "arr[" << i << "] = ";
        }
    }
        return a;
}

// Шаблон ФУНКЦИИ для поиска минимального нечетного элемента
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

// Шаблон функции для вывода результата поиска минимального нечтного элемента
template <class T>
void minOddResult(T min){
    if (min % 2 == 0) cout << "Минимальный нечетный элемент не найден" << endl;
       else cout << "Минимальный нечетный элемент: " << min << endl;
}
template <class T>
void printTypeMenu(deque<T>* objDec){
    int stillWorkMenu = 1;
    int choiceMenu;
    do{
        system("CLS");
        cout << "1. Добавление элемента в начало." << endl;
        cout << "2. Добавление элемента в конец." << endl;
        cout << "3. Удаление элемента из начала." << endl;
        cout << "4. Удаление элемента из конца." << endl;
        cout << "5. Значение первого элемента в деке." << endl;
        cout << "6. Значение последнего элемента в деке." << endl;
        cout << "7. Очистить дек." << endl;
        cout << "8. Назад." << endl;
        cout << "Выберите пункт меню: ";
        while (!(cin >> choiceMenu) || choiceMenu < 1  || choiceMenu > 8)
            {
                cleaning();
                cout << endl << "Ошибка: выбран несуществующий пункт меню." << endl << endl;
                cout << "Пункт меню: ";
            }
        system("CLS");
        switch(choiceMenu){
            case 1:
            {
                T num;
                cout << "Введите значение для элемента: ";
                cin >> num;
                objDec->push_front(num);
                cout << "Элемент успешно добавлен в дек." << endl;
                break;
            }
            case 2:
            {
                T num;
                cout << "Введите значение для элемента: ";
                cin >> num;
                objDec->push_back(num);
                cout << "Элемент успешно добавлен в дек." << endl;
                break;
            }
            case 3:
                if(objDec->size())
                {
                   cout << "Удаление элемента: " << objDec->front() << endl;
                   objDec->pop_front();
                } else cout << "Пусто..." << endl;
                break;
            case 4:
                if(objDec->size())
                {
                   cout << "Удаление элемента: " << objDec->back() << endl;
                   objDec->pop_back();
                } else cout << "Пусто..." << endl;
                break;
            case 5:
                if(objDec->size())
                {
                    cout << "Первый элемент в деке: " << objDec->front() << endl;
                } else cout << "Пусто..." << endl;
                break;
            case 6:
                if(objDec->size())
                {
                   cout << "Последний элемент в деке: " << objDec->back() << endl;
                } else cout << "Пусто..." << endl;
                break;
            case 7:
                if(objDec->size())
                {
                    objDec->clear();
                    cout << "Все элементы дека успешно удалены." << endl;
                } else cout << "Зачем очищать пустой дек???? -_- " << endl;
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
            cout << "\n1. Поиск минимального нечетного элемента массива." << endl;
            cout << "2. АТД Дек." << endl;
            cout << "3. Выход." << endl;
            cout << "\nВведите пункт меню: ";
            while (!(cin >> choiceMenu) || choiceMenu < 1  || choiceMenu > 3)
                {
                    cleaning();
                    cout << endl << "Ошибка: выбран несуществующий пункт меню." << endl << endl;
                    cout << "Пункт меню: ";
                }
            system("CLS");
            switch(choiceMenu){
                case 1:
                    cout << "Введите размер массива: ";
                    while (!(cin >> arrSize) || arrSize < 0)
                        {
                            cleaning();
                            cout << endl << "Ошибка: задан некорректный размер массива." << endl << endl;
                            cout << "Размер массива: ";
                        }
                    cout << "1. Тип аргумента int." << endl;
                    cout << "2. Тип аргумента unsigned long." << endl;
                    cout << "Выберите тип аргумента: ";
                    while (!(cin >> choiceMenu) || choiceMenu < 1  || choiceMenu > 2)
                        {
                            cleaning();
                            cout << endl << "Ошибка: выбран несуществующий пункт меню." << endl << endl;
                            cout << "Пункт меню: ";
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
                cout << "1. Тип аргумента int." << endl;
                cout << "2. Тип аргумента unsigned long." << endl;
                cout << "Выберите тип аргумента: ";
                while (!(cin >> choiceMenu) || choiceMenu < 1  || choiceMenu > 2)
                    {
                        cleaning();
                        cout << endl << "Ошибка: выбран несуществующий пункт меню." << endl << endl;
                        cout << "Пункт меню: ";
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
                    cout << "Завершение..." << endl;
                    stillWorkMenu = 0;
                    break;
            }
            system("pause");
    }while(stillWorkMenu);
    return 0;
}
