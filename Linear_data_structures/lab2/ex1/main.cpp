#include <stdio.h>
#include <stdlib.h>
#include <locale>
#include <iostream>

using namespace std;


/*������� n �������. ��������� �� ��� ��������� �������� ��������� �����.
������ �� �������������, ����������� �� ������ �, �������, ����� ������� �� ������ �, ���� � ���� � � �������, � ������ � � �� 100 ��. �, �, � � � �������� � ����������.*/

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
    // ������� ����
    struct graph* g = new struct graph;

    // ���-�� ������
    g->numVertices = NumVertexes;

    // ������� ������ ������
    g->list = new struct node*[NumVertexes];

    // �������������� ������
    for (int i = 0; i < NumVertexes; i++){
        g->list[i] = NULL;
    }
    return g;
}

// �������� ������
struct node* createNode(int num, int w)
{
    struct node* newNode = new struct node;
    // ����� �������
    newNode->numberNode = num;
    // ���
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


// �������� �������� ����� ������� ���������
void DijkstraArray (int start, int D[], int P[], int ** array, int numV)
{
      // D ����������� ����������
      // P ���������� �������
      int temp, minindex, min;
      for (int i = 0; i < numV; i++)
      {
        D[i] = 9999;
        P[i] = 1;
      }
      D[start] = 0;
      // ��� ���������
      do {
        minindex = 9999;
        min = 9999;
        for (int i = 0; i < numV; i++)
        { // ���� ������� ��� �� ������ � ��� ������ min
          if ((P[i] == 1) && (D[i] < min))
          {
            // ��������������� ��������
            min = D[i];
            minindex = i;
          }
        }
        // ��������� ��������� ����������� ���
        // � �������� ���� �������
        // � ���������� � ������� ����������� ����� �������
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
      // �������������� ����
      int ver[numV]; // ������ ���������� ������
      ver[0] = end; // ��������� ������� - �������� �������
      int k = 1; // ������ ���������� �������
      int weight = D[end]; // ��� �������� ������� 

      while (end != start) // ���� �� ����� �� ��������� ������� begin_index
      {
        for (int i = 0; i < numV; i++)// ������������� ��� �������
        {
            if(i > end)
            {
                if (array[i][end] != 0)   // ���� ����� ����
                {
                  int temp = weight - array[i][end];// ���������� ��� ���� �� ���������� �������
                  if (temp == D[i]) // ���� ��� ������ � ������������
                  {                 // ������ �� ���� ������� � ��� �������
                    weight = temp; // ��������� ����� ���
                    end = i;       // ��������� ���������� �������
                    ver[k] = i; // � ���������� �� � ������
                    k++;
                  }
                }
            }
            else
            {
                if (array[end][i] != 0)   // ���� ����� ����
                {
                  int temp = weight - array[end][i];// ���������� ��� ���� �� ���������� �������
                  if (temp == D[i]) // ���� ��� ������ � ������������
                  {                 // ������ �� ���� ������� � ��� �������
                    weight = temp; // ��������� ����� ���
                    end = i;       // ��������� ���������� �������
                    ver[k] = i; // � ���������� �� � ������
                    k++;
                  }
                }
            }
        }
      }
      // ����� ���� (��������� ������� ��������� � ����� ������� �� k ���������)
      cout << endl << "������� ��������� " << endl;
      cout << "���������� ����: ";
      for (int i = k - 1;  i >= 0; i--)
      {
           cout << ver[i];
           if (i != 0){
              cout << " -> ";
          }
      }
      cout << endl;
}

// ���������� �����
void addNodeTop(struct graph* g, int src, int dest, int weight)
{
    struct node* newNode = createNode(dest, weight);
    newNode->next = g->list[src];
    g->list[src] = newNode;
}

void pathList(int D[], struct graph* graph, int src, int dst, int numV)
{
      // �������������� ����
      int ver[numV]; // ������ ���������� ������
      ver[0] = dst; // ��������� ������� - �������� �������
      int k = 1; // ������ ���������� �������
      int weight = D[dst]; // ��� �������� �������

      while (dst != src) // ���� �� ����� �� ��������� �������
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

      // ����� ���� (��������� ������� ��������� � ����� ������� �� k ���������)
      cout << endl << "������ ��������� " << endl;
      cout << "���������� ����: ";
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
    int D[numV], P[numV]; // D - ����������� ����������   P -  ���������� �������
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
            cout << endl << "������������� ������ �������, ����� ������� �� ������ " << src << " � ����� " << dst << endl;
        } else cout << endl << "������������� ���-�� �������" << endl;
    }
    else cout << "C����� ��������� " << endl <<"��� ���� �� " << src << " �� " << dst << endl;
}

// ����� ������ ���������
void printGraph(struct graph* graph)
{
    int v;
    for (v = 0; v < graph->numVertices; v++)
    {
        struct node* temp = graph->list[v];
        printf("\n ������� %d\n ", v);
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
    cout << "������� ����� ������ - ������: " << endl;
    cin >> A;
    cout << "������� ����� ������ - ��������: " << endl;
    cin >> B;
    cout << "������� ���������� ������ ������� � ����: " << endl;
    cin >> x;
    cout << "������� ������ ������� �� 100��: " << endl;
    cin >> y;
    // ������ ���������
    dijkstra(g, A, B, x , y);

    // ������� ���������
    DijkstraArray(A, D, P, array, numV);

    if (D[B] < 9999){
        pathArray (D, array, B,A, numV);
        if(x / y  * 100 >= D[B]){
            cout << endl  << "������������� ������ �������, ����� ������� �� ������ " << A << " � ����� " << B << endl;
        } else cout << endl << "������������� ���-�� �������" << endl;
    }
      else
        cout << "������� ��������� " << endl << "��� ���� �� " << A << " �� " << B << endl;
}

int workFile(const char* fileName)
{
    int n = 0; // ���-�� ������ �����


    FILE* f = fopen(fileName, "r");
    if (f == NULL)
    {
        puts("Error");
        return -1;
    }


    // ��������� ���-�� ������ � �����

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
        // ������ ���������
        struct graph* g = createGraph(n); // ������� ���� �� n ������
        // ������� ���������
        int** a = new int*[n];
        for (int i = 0, size = 1; i < n; i++)
        {
            *(a+i) = new int[size++];
        }
        int src = 0;
        int dst = 0;
        int w = 0; // ���
        bool writeToArray = true;
        while (!feof(f))
        {
                //����� �������� �� fIn ��� ����� ������� �������� ����� ��� � ��++
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

        // ����� ������ ��������� � ������� ���������
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
    } else cout << " ���� ���� ��� ����� �����������." << endl;

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
