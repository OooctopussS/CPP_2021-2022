#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <random>
using namespace std;

typedef unsigned char uchar;
typedef unsigned short int ushort;

typedef struct operationCounter
{
    unsigned long long int equalCount; // Счетчик присваивания
    unsigned long long int equalArrayCount; // Счетчик работы с массивом
}operation;

// Сортировка с помощью прямого выбора (метод минимума)
void StraightSelection (int* &a, int n, bool isNotLastRound, operation& counter)
{
    counter. equalArrayCount = 0;
    counter.equalCount = 0;
    int i, j, tmp, *min;
    for (counter.equalCount++, i = 0; i < n - 1; counter.equalCount++, i++)
    {
        min = a + i; counter.equalArrayCount++;
        for (counter.equalCount++,  j = i + 1; j < n; counter.equalCount++, j++)
        {
            if (isNotLastRound ? a[j] > *min : a[j] < *min)
            {
                min = a + j; counter.equalArrayCount++;
            }
        }
        tmp = *min; counter.equalCount++;
        *min = a[i]; counter.equalArrayCount++;
        a[i] = tmp; counter.equalArrayCount++;
    }
}

// Пузырьковая сортировка с фиксацией места обмена
void BubbleSort (int* &a, int n, bool isNotLastRound, operation& counter)
{
    counter.equalArrayCount = 0;
    counter.equalCount = 0;
    int i = 1, j, x, k;	/*i=1 - левая граница сравниваемых элементов*/
    do
    {
        k = 0; counter.equalCount++;	/*признак отсутствия обменов*/
        for (counter.equalCount++, j = n - 1; j >= i; counter.equalCount++, j--)
        {
            if (isNotLastRound ? a[j-1] < a[j] : a[j-1] > a[j])
            {
                x = a[j-1]; counter.equalArrayCount++;
                a[j-1] = a[j]; counter.equalArrayCount++;
                a[j] = x; counter.equalArrayCount++;
                k = j; counter.equalCount++;	/*запоминаем место последнего обмена*/
            }
        }
        i = k; counter.equalCount++;	/*запоминаем место последнего обмена как левую границу*/
    }
    while (k);
}

// Сортировка с двоичным включением (бинарными вставками)
void BinaryInsertion (int* &a, int n, bool isNotLastRound, operation& counter)
{
    counter.equalArrayCount = 0;
    counter.equalCount = 0;
    int i, j, m, left, right, x;
    for (counter.equalCount++, i = 1; i < n;counter.equalCount++,i++)
    {
        x = a[i]; counter.equalArrayCount++; left = 0; counter.equalCount++;right = i; counter.equalCount++;
        while (left < right)	/*пока есть что просматривать*/
        {
            m = left+(right-left)/2; counter.equalCount++;	/* делим интервал поиска пополам*/
            if (isNotLastRound ? a[m] >= x : a[m] <= x)	/*если вставляемое значение меньше текущего */
                {
                left = m + 1;counter.equalCount++;/*искать надо в правой части*/
                }
            else
            {
                right = m; counter.equalCount++;		/*а если нет, то будем искать в левой*/
            }
        }
        for (counter.equalCount++,j = i; j > right;counter.equalArrayCount++, a[j] = a[j-1], counter.equalCount++,j--);
        a[right]=x; counter.equalArrayCount++;
    }
}

void swap (int &a, int &b)
{
     int tmp = a;
     a = b;
     b = tmp;
}

// Создать счетчики.
// data-сортируемый массив, counters-массив для счетчиков, N-число элементов в data
void createCounters(int *data, long *counters, long N) {
    // i-й массив count расположен, начиная с адреса counters+256*i
    memset( counters, 0, 256 * sizeof(int)*sizeof(long) );

    uchar *bp = (uchar*)data;
    uchar *dataEnd = (uchar*)(data + N);
    ushort i;

    while (bp != dataEnd) {
        // увеличиваем количество байт со значением *bp
        // i - текущий массив счетчиков
        for (i=0; i < sizeof(int);i++)
            counters[256 * i + *bp++]++;
    }
}


// Функция radixPass принимает в качестве параметров
// номер байта Offset,
// число элементов N,
// исходный массив source,
// массив dest, куда будут записываться числа, отсортированные по байту Offset
// массив счетчиков count, соответствующий текущему проходу.

void radixPass (short Offset, long N, int *source, int *dest, long *count, bool isNotLastRound,operation& counter) {
    // временные переменные
    int *sp;
    long s, c, i, *cp;
    uchar *bp;


    // шаг 3
    counter.equalCount++;
    s = 0; 	// временная переменная, хранящая сумму на данный момент
    counter.equalCount++;
    cp = count;
    for (counter.equalCount++, i = 256; i > 0;counter.equalCount++, --i,counter.equalCount++, ++cp) {
        c = *cp; counter.equalCount++;
        *cp = s; counter.equalCount++;
        s += c; counter.equalCount++;
    }

    // шаг 4
    bp = (uchar *)source + Offset; counter.equalCount++;
    sp = source; counter.equalCount++;
    for (counter.equalCount++, isNotLastRound ? i = N : i = 0; isNotLastRound ? i > 0 : i < N;counter.equalCount++, isNotLastRound ? --i : ++i,counter.equalCount++, bp += sizeof(int) ,counter.equalCount++, ++sp)
    {
        cp = count + *bp; counter.equalCount++;
        dest[*cp] = *sp;counter.equalArrayCount++;
        ++(*cp);counter.equalCount++;
    }
}

// сортируется массив in из N элементов
// T - любой беззнаковый целый тип
void radixSort (int* &in, int N, bool isNotLastRound, operation& counter) {
    counter.equalArrayCount = 0;
    counter.equalCount = 0;
    int *out = new int[N];
    long *counters = new long[sizeof(int) * 256], *count;
    createCounters(in, counters, N);
    ushort i;
    for (counter.equalCount++, i = 0; i < sizeof(int);counter.equalCount++, i++) {
        counter.equalCount++;
        count = counters + 256 * i;         // count - массив счетчиков для i-го разряда
        counter.equalCount++;
        if ( count[0] == N ) continue;    // (*** см ниже)
        counter.equalCount++; counter.equalCount++; counter.equalCount++; counter.equalCount++; counter.equalCount++;
        radixPass (i, N, in, out, count, isNotLastRound, counter); // после каждого шага входной и
        counter.equalCount++; counter.equalCount++; counter.equalCount++; counter.equalCount++; counter.equalCount++;
        swap(in, out);                    // выходной массивы меняются местами
    }
                          // по окончании проходов
    delete[] out;           // вся информация остается во входном массиве.
    delete[] counters;
}




int readFile(string filename, const int& N, int* &array)
{
    ifstream file(filename);
    if (!file.is_open())
    {
       cout << "The file cannot be opened!\n";
       return 0;
    }
    else
    {
        for(int i = 0; i < N; i++)
        {
            file >> array[i];
        }
    }
    file.close();
    return 1;
}

int main()
{
    void (*func[4])(int* &, int, bool, operation&) = {StraightSelection, BubbleSort , BinaryInsertion, radixSort};
    unsigned int numbers[4] = {10000, 30000, 50000, 90000};
    string str[4] = {"N410.txt", "N4100.txt", "N4500.txt", "N41000.txt"};
    int timer;
    operation counter;
    bool isNotLastRound;

    for(int i = 0; i < 4; i++)
    {
        for(int k = 0; k < 4; k++)
        {
            int* arr = new  int[numbers[i]];
            readFile("test_numbers.txt", numbers[i], arr);
            isNotLastRound = true;
            cout << endl;
            cout << "----------------------------------------------------" << endl << "| ";
            cout.width(17);
            if (k == 0) cout << "StraightSelection";
            else if(k == 1) cout << "BubbleSort";
            else if(k == 2) cout << "BinaryInsertion";
            else cout << "Radix sort";
            cout << "        |" << "        N = " << numbers[i] << "      |" << endl;
            cout << "----------------------------------------------------" << endl;
            cout << "|  time  |      equal      |    array operation    |" << endl;
            cout << "----------------------------------------------------" << endl;
            for(int j = 0; j < 3; j++)
            {
                if(j == 2) isNotLastRound = false;
                timer = clock();
                func[k](arr, numbers[i], isNotLastRound, counter);
                timer = clock() - timer;
                cout << "|" << setw(6) << timer << "  ";
                cout << "|  " << setw(12) << counter.equalCount << "   ";
                cout << "|  " << setw(15) << counter. equalArrayCount << "      |";
                cout << endl << "----------------------------------------------------" << endl;
            }
             delete[] arr;
        }
    }

    cout  << endl << endl << "REPEAT KEYS!!" << endl;
    for(int i = 0; i < 4; i++)
    {
        for(int k = 0; k < 4; k++)
        {
            int* arr = new  int[numbers[3]];
            readFile(str[i], numbers[3], arr);
            isNotLastRound = true;
            cout << endl;
            cout << str[i] << endl;
            cout << "----------------------------------------------------" << endl << "| ";
            cout.width(17);
            if (k == 0) cout << "StraightSelection";
            else if(k == 1) cout << "BubbleSort";
            else if(k == 2) cout << "BinaryInsertion";
            else cout << "Radix sort";
            cout << "        |" << "        N = " << numbers[3] << "      |" << endl;
            cout << "----------------------------------------------------" << endl;
            cout << "|  time  |      equal      |    array operation    |" << endl;
            cout << "----------------------------------------------------" << endl;
            for(int j = 0; j < 3; j++)
            {
                if(j == 2) isNotLastRound = false;
                timer = clock();
                func[k](arr, numbers[3], isNotLastRound, counter);
                timer = clock() - timer;
                cout << "|" << setw(6) << timer << "  ";
                cout << "|  " << setw(12) << counter.equalCount << "   ";
                cout << "|  " << setw(15) << counter. equalArrayCount << "      |";
                cout << endl << "----------------------------------------------------" << endl;
            }
             delete[] arr;
        }
    }

    system("pause");
    return 0;
}

