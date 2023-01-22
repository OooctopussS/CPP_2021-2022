#include <stdio.h>
#include <stdlib.h>
#include <locale>
#include <iostream>

using namespace std;


/*Имеются n городов. Некоторые из них соединены дорогами известной длины.
Хватит ли дальнобойщику, выезжающему из города А, топлива, чтобы доехать до города В, если в баке х л бензина, а расход у л на 100 км. А, В, х и у вводятся с клавиатуры.*/

struct node
{
    int numberNode;
    int weight;
    struct node* next;
};

struct graph
{
    int numVertices;
    struct node** list;
};

struct graph* createGraph(int NumVertexes){
    // Создаем граф
    struct graph* g = new struct graph;

    // Кол-во вершин
    g->numVertices = NumVertexes;

    // Создаем список вершин
    g->list = new struct node*[NumVertexes];

    // Инициализируем список
    for (int i = 0; i < NumVertexes; i++){
        g->list[i] = NULL;
    }
    return g;
}

// Создание записи
struct node* createNode(int num, int w)
{
    struct node* newNode = new struct node;
    // Номер вершины
    newNode->numberNode = num;
    // Вес
    newNode->weight = w;
    newNode->next = NULL;
    return newNode;
}

void deleteGraph(struct graph* g, int v)
{
    for (int i = 0; i < v; i++){
        g->list[i] = NULL;
        delete g->list[i];
    }
    delete[] g->list;
    delete g;
}

void deleteArray(int** array, int v)
{
    for(int i = 0; i < v; i++){
        delete[] *(array+i);
    }
    delete[] array;
}


// Алгоритм Дейкстра через матрицу смежности
void DijkstraArray (int start, int D[], int P[], int ** array, int numV)
{
      // D минимальное расстояние
      // P посещенные вершины
      int temp, minindex, min;
      for (int i = 0; i < numV; i++)
      {
        D[i] = 9999;
        P[i] = 1;
      }
      D[start] = 0;
      // Шаг алгоритма
      do {
        minindex = 9999;
        min = 9999;
        for (int i = 0; i < numV; i++)
        { // Если вершину ещё не обошли и вес меньше min
          if ((P[i] == 1) && (D[i] < min))
          {
            // Переприсваиваем значения
            min = D[i];
            minindex = i;
          }
        }
        // Добавляем найденный минимальный вес
        // к текущему весу вершины
        // и сравниваем с текущим минимальным весом вершины
        if (minindex != 9999)
        {
          for (int i = 0; i < numV; i++)
          {
                if(i <= minindex)
                {
                    if (array[minindex][i] > 0)
                    {
                      temp = min + array[minindex][i];
                      if (temp < D[i])
                      {
                        D[i] = temp;
                      }
                    }
                }
                else
                {
                    if (array[i][minindex] > 0)
                    {
                      temp = min + array[i][minindex];
                      if (temp < D[i])
                      {
                        D[i] = temp;
                      }
                    }
                }
          }
          P[minindex] = 0;
        }
      } while (minindex < 9999);
}

void pathArray(int D[], int **array, int end, int start, int numV)
{
      // Восстановление пути
      int ver[numV]; // массив посещенных вершин
      ver[0] = end; // начальный элемент - конечная вершина
      int k = 1; // индекс предыдущей вершины
      int weight = D[end]; // вес конечной вершины 

      while (end != start) // пока не дошли до начальной вершины begin_index
      {
        for (int i = 0; i < numV; i++)// просматриваем все вершины
        {
            if(i > end)
            {
                if (array[i][end] != 0)   // если связь есть
                {
                  int temp = weight - array[i][end];// определяем вес пути из предыдущей вершины
                  if (temp == D[i]) // если вес совпал с рассчитанным
                  {                 // значит из этой вершины и был переход
                    weight = temp; // сохраняем новый вес
                    end = i;       // сохраняем предыдущую вершину
                    ver[k] = i; // и записываем ее в массив
                    k++;
                  }
                }
            }
            else
            {
                if (array[end][i] != 0)   // если связь есть
                {
                  int temp = weight - array[end][i];// определяем вес пути из предыдущей вершины
                  if (temp == D[i]) // если вес совпал с рассчитанным
                  {                 // значит из этой вершины и был переход
                    weight = temp; // сохраняем новый вес
                    end = i;       // сохраняем предыдущую вершину
                    ver[k] = i; // и записываем ее в массив
                    k++;
                  }
                }
            }
        }
      }
      // Вывод пути (начальная вершина оказалась в конце массива из k элементов)
      cout << endl << "Матрица смежности " << endl;
      cout << "Кратчайший путь: ";
      for (int i = k - 1;  i >= 0; i--)
      {
           cout << ver[i];
           if (i != 0){
              cout << " -> ";
          }
      }
      cout << endl;
}

// Добавление ребер
void addNodeTop(struct graph* g, int src, int dest, int weight)
{
    struct node* newNode = createNode(dest, weight);
    newNode->next = g->list[src];
    g->list[src] = newNode;
}

void pathList(int D[], struct graph* graph, int src, int dst, int numV)
{
      // Восстановление пути
      int ver[numV]; // массив посещенных вершин
      ver[0] = dst; // начальный элемент - конечная вершина
      int k = 1; // индекс предыдущей вершины
      int weight = D[dst]; // вес конечной вершины

      while (dst != src) // пока не дошли до начальной вершины
      {
          struct node* tempNode = graph->list[dst];
          while (tempNode)
          {
              int temp = weight - tempNode->weight;
              if (temp == D[tempNode->numberNode])
              {
                  weight = temp;
                  dst = tempNode->numberNode;
                  ver[k] = tempNode->numberNode;
                  k++;
              }
              tempNode = tempNode->next;
          }
      }

      // Вывод пути (начальная вершина оказалась в конце массива из k элементов)
      cout << endl << "Список смежности " << endl;
      cout << "Кратчайший путь: ";
      for (int i = k - 1;  i >= 0; i--)
      {
           cout << ver[i];
           if (i != 0){
              cout << " -> ";
          }
      }
      cout << endl;
}

void dijkstra(struct graph* graph, int src, int dst, int x, int y)
{
    int numV = graph->numVertices;
    int D[numV], P[numV]; // D - минимальное расстояние   P -  посещенные вершины
    int temp, minindex, min;
    for (int i = 0; i < numV; i++)
    {
        D[i] = 9999;
        P[i] = 1;
    }
    D[src] = 0;

    do{
      minindex = 9999;
      min = 9999;
      for (int i = 0; i < numV; i++)
      {
          if ((P[i] == 1) && (D[i] < min))
          {
              min = D[i];
              minindex = i;
          }
      }

      if (minindex != 9999)
      {
             struct node* tempNode = graph->list[minindex];
             while (tempNode)
             {
                 temp = min + tempNode->weight;
                 if (temp < D[tempNode->numberNode])
                 {
                     D[tempNode->numberNode] = temp;
                 }
                 tempNode = tempNode->next;
             }
         P[minindex] = 0;
      }
    }while(minindex < 9999);

    if(D[dst] < 9999)
    {
     //   pathList(D, graph, src, dst, numV);
        if(x / y  * 100 >= D[dst]){
            cout << endl << "Дальнобойщику хватит топлива, чтобы доехать из города " << src << " в город " << dst << endl;
        } else cout << endl << "Недостаточное кол-во топлива" << endl;
    }
    else cout << "Cписок смежности " << endl <<"Нет пути от " << src << " до " << dst << endl;
}

// Вывод списка смежности
void printGraph(struct graph* graph)
{
    int v;
    for (v = 0; v < graph->numVertices; v++)
    {
        struct node* temp = graph->list[v];
        printf("\n Смежные %d\n ", v);
        while (temp)
        {
            printf("%d -> ", temp->numberNode);
            temp = temp->next;
        }
        printf("\n");
    }
}

void infoAboutRoad(int** array, struct graph* g, int numV){
    int D[numV], P[numV];
    int A, B;
    double x,y;
    cout << "Введите номер города - старта: " << endl;
    cin >> A;
    cout << "Введите номер города - конечный: " << endl;
    cin >> B;
    cout << "Введите количество литров бензина в баке: " << endl;
    cin >> x;
    cout << "Введите расход бензина на 100км: " << endl;
    cin >> y;
    // Список смежности
    dijkstra(g, A, B, x , y);

    // Матрица смежности
    DijkstraArray(A, D, P, array, numV);

    if (D[B] < 9999){
        pathArray (D, array, B,A, numV);
        if(x / y  * 100 >= D[B]){
            cout << endl  << "Дальнобойщику хватит топлива, чтобы доехать из города " << A << " в город " << B << endl;
        } else cout << endl << "Недостаточное кол-во топлива" << endl;
    }
      else
        cout << "Матрица смежности " << endl << "Нет пути от " << A << " до " << B << endl;
}

int workFile(const char* fileName)
{
    int n = 0; // Кол-во вершин графа


    FILE* f = fopen(fileName, "r");
    if (f == NULL)
    {
        puts("Error");
        return -1;
    }


    // считываем кол-во вершин в файле

    char str[BUFSIZ];
    fgets(str, BUFSIZ, f);
    for (int i = 0; str[i]; i++)
    {
        if(isdigit(str[i]) && str[i-1] == ' ') n++;
    }
    n++;

    rewind(f);

    if (n != 0)
    {
        // список смежности
        struct graph* g = createGraph(n); // создаем граф из n вершин
        // матрица смежности
        int** a = new int*[n];
        for (int i = 0, size = 1; i < n; i++)
        {
            *(a+i) = new int[size++];
        }
        int src = 0;
        int dst = 0;
        int w = 0; // Вес
        bool writeToArray = true;
        while (!feof(f))
        {
                //можно заменить на fIn для этого сделать открытие файла как в си++
                fscanf(f, "%d", &w);
                if(w != 9999 && w != 0)
                {
                  addNodeTop(g,src,dst,w);
                }

                if (writeToArray)
                {
                    a[src][dst] = w;
                }
                if (dst == n - 1) writeToArray = true;
                if (w == 0) writeToArray = false;

                if(dst == n - 1){
                src++;
                dst = 0;
                } else dst++;
        }

        // Вывод списка смежности и матрицы смежности
       /*    printGraph(g);

        int j = 1;
        for (int i = 0; i < n; i++)
        {
            for (int k = 0; k < j; k++)
                    cout << a[i][k] << "    ";
            j++;
            cout << endl;
        }*/


        infoAboutRoad(a, g, n);


        deleteArray(a, n);
        deleteGraph(g, n);
    } else cout << " Граф пуст или задан неправильно." << endl;

    fclose(f);
    return 0;
}

int main()
{
    setlocale(LC_ALL, "Rus");

    workFile("graph.txt");

    system("pause");
    return 0;
}
