#include <iostream>
#include <list>
#include <fstream>
#include <stdlib.h>
#include <cstdio>
#include <cstring>

using namespace std;

typedef struct hashAdressArray
{
    int key;
    char condition; // 0 -> слот свободен 1 -> занят 2 -> удален
}hArray;

// Хеш-таблица с разрешением коллизий методом цепочек
class Hash
{
private:
    int bucket;    // Количество ковшей
    // Указатель на массив, содержащий сегменты
    list<int> *table;
    int numbersOfComparisons; // кол-во сравнений для поиска ключа
public:

    // Конструктор
    Hash(int b = 100): numbersOfComparisons(0)
    {
        while(!simple(b))
        {
            b++;
        }
        bucket = b;
        table = new list<int>[bucket];
    }

    bool simple(int n){
        for(int i=2;i<= n/2;i++) if( (n%i) == 0 ) return false;
    return true;
    }

    // + ключ в таблицу
    void insertItem(int key)
    {
        int index = hashFunction(key);
        table[index].push_back(key);
    }

    bool find(int key)
    {
        numbersOfComparisons = 0;
        // получаем индекс ключа
        int index = hashFunction(key);
        // ищем ключ
        list <int> :: iterator i;
        for (i = table[index].begin();numbersOfComparisons++, i != table[index].end(); i++)
        {
          numbersOfComparisons++;
          if (*i == key)
            return true;
        }
        return false;
    }

    int getNumberOfComparisons()
    {
       return numbersOfComparisons;
    }

    // - ключ в таблице
    void deleteItem(int key)
    {
      int index = hashFunction(key);
      list <int> :: iterator i;
      for (i = table[index].begin(); i != table[index].end(); i++)
      {
        if (*i == key)
          break;
      }

      // если ключ найден в таблице, удалить его
      if (i != table[index].end())
        table[index].erase(i);
    }

    //  функция для сопоставления значений с ключом(метод деления)
    int hashFunction(int x) {
        return (x % bucket);
    }

    void displayHash() {
      for (int i = 0; i < bucket; i++) {
        cout << i;
        for (auto x : table[i])
          cout << " --> " << x;
        cout << endl;
      }
    }
};

class HashAdress{
private:
    int size;
    hArray* k;
    int numbersOfComparisons; // кол-во сравнений для поиска ключа
public:

    HashAdress(int s = 100): numbersOfComparisons(0)
    {
        s *= 2;
        while(!simple(s))
        {
            s++;
        }
        size = s;
        k = new hArray[size];
        for(int i = 0; i < size; i++)
        {
            k[i].condition = '0';
        }
    }

    bool simple(int n){
        for(int i=2;i<= n/2;i++) if( (n%i) == 0 ) return false;
    return true;
    }

    int h(int x)
    {
        return (x % size);
    }

    int rh(int i)
    {
        return (i + 1) % size;
    }

    int getNumberOfComparisons()
    {
       return numbersOfComparisons;
    }

    void insertItem(int key)
    {
        int i;
        i = h(key);

        while((k[i].key != key) && (k[i].condition != '0'))
        {
            i = rh(i);
        }

      /*  if(k[i].key == i) /////
        {
            k[i].key = key;
        }*/
        if(k[i].condition == '0') /////
        {
                    k[i].key = key;
                    k[i].condition = '1';
        }
    }

    bool find(int key)
    {
           // Применяем хеш-функцию для поиска индекса для данного ключа
           numbersOfComparisons = 0;
           int hashIndex = h(key);

           // найти узел с заданным ключом

            while(numbersOfComparisons++, k[hashIndex].condition == '1')
            {
                numbersOfComparisons++;
                if(k[hashIndex].key == key)
                {
                   return true;
                }
                else
                {
                    hashIndex = rh(hashIndex);
                }
            }

           return false;
    }

    void deleteNode(int key)
    {
            // Применяем хеш-функцию для поиска индекса для данного ключа
            int hashIndex = h(key);

            // найти узел с заданным ключом
            while(k[hashIndex].key != NULL)
            {
                // если узел найден
                if(k[hashIndex].key == key)
                {
                    k[hashIndex].key = NULL;
                    k[hashIndex].condition = '2';
                }
                hashIndex = rh(hashIndex);
            }
        }

    ~HashAdress()
    {
        delete[] k;
    }
};

int readFile(string filename, Hash &hash, HashAdress &hAdr)
{
    ifstream file(filename);
    if (!file.is_open())
    {
       cout << "The file cannot be opened!\n";
       return 0;
    }
    else
    {
        int a;
        while(!file.eof())
        {
            file >> a;
            hash.insertItem(a);
            hAdr.insertItem(a);
        }
    }
    file.close();
    return 1;
}

int readFileCount(string filename, int &N)
{
    ifstream file(filename);
    if (!file.is_open())
    {
       cout << "The file cannot be opened!\n";
       return 0;
    }
    else
    {
        int a;
        while(!file.eof())
        {
            if(file >> a) N++;
        }
    }
    file.close();
    return 1;
}

// создать массив размером с кол-вом элементов

int main()
{
  int arr[] = {69066263, 95229554, 55559212, 76931203, 10692164, 10446233, 60850261, 14487016, 22858511, 81329317, 43621505, 31100897, 10100708, 63376844}; // ключи которые есть в программе 100%
  int N = 0;
  readFileCount("test_numbers.txt", N);
  Hash h(N);
  HashAdress hA(N);
  readFile("test_numbers.txt", h, hA);
  int keyForFind;
  int sumComparisonsOne = 0, sumComparisonsTwo = 0;
  for(int i = 0; i < 100; i++)
  {
      if(i < 14) keyForFind = arr[i];  // берем значения, которые есть в таблице(чтобы были 100% найденные ключи)
      else if(i < 46) keyForFind = rand() % 9999999;
      else if(i < 86) keyForFind = 10000000+ rand() % 99999999;
      else  keyForFind = 100000000 + rand();
      if(h.find(keyForFind))
      {
          cout << "key found" << endl;
          cout << "number of comparisons: " << h.getNumberOfComparisons() << endl;
      }
      else
      {
          cout << "key not found" << endl;
          cout << "number of comparisons: " << h.getNumberOfComparisons() << endl;
      }

      cout << endl << "Adress" << endl;
      if(hA.find(keyForFind))
      {
          cout << "key found" << endl;
          cout << "number of comparisons: " << h.getNumberOfComparisons() << endl;
      }
      else
      {
          cout << "key not found" << endl;
          cout << "number of comparisons: " << h.getNumberOfComparisons() << endl;
      }
      sumComparisonsOne += h.getNumberOfComparisons();
      sumComparisonsTwo += hA.getNumberOfComparisons();
      cout << endl;
  }
  cout << "average number of comparisons: " << sumComparisonsOne / 100 << endl;
  cout << "Adress average number of comparisons: " << sumComparisonsTwo / 100 << endl;

  return 0;

}

