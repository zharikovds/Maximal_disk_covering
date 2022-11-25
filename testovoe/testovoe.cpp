#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ---------------------------------------------------------------------------------------------------------
// Описание программы:
// 1)Считываем из файла список координат целей и записываем в список элементов target
// 
// 2)Определяем квадрат расстояния между точками и сохраняем в список элементов coord
// 
// 3)Сортируем список coord'ов по возрастанию квадрата расстояния и убираем те элементы из списка,
// у которых квадрат расстояния больше квадрата диаметра
// 
// 4)Строим окружность, точки хорды которой соответствуют значениям, записанным в элементах списка coord'ов
// 
// 5)Находим центр окружности из шага 4
// 
// 6)Проверяем все точки из списка элементов target на принадлежность к окружности из шага 4
// 
// 7)Выбираем окружность с наибольшим количеством точек внутри
// ----------------------------------------------------------------------------------------------------------

typedef struct target{
    int x, y; // Координаты цели
    target* next; 
}target;

// Функции для работы со структурой target
target* AddTarget(target* head); // Добавление цели в список
void DeleteTarget(target* target_to_delete, target* head); // Удаление цели из списка
void ClearTargets(target* head); // Удаление списка целей
void PrintTargets(target* head); // Вывод списка целей

typedef struct coord{
    int x0, y0; // Координаты первой точки
    int x, y; // Координаты второй точки
    int dist_sqr; // Квадрат расстояния между А0 и А
    coord* next;
}coord;

// Функции для работы со структурой coord
coord* AddCoord(coord* head, target* tgf, target* tgs); // Добавление координат с координатами цели
coord* AddCoord(coord* head); // Добавление координат без координат цели
void CreateList(coord* head, target* thead); // Создание списка координат
void DeleteCoord(coord* coord_to_delete,coord* head); // Удаление списка координат
coord* DeleteCoordHead(coord* head); // Удаление первого элемента списка координат
void FilterCoord(coord* head, int rad); // Фильтр списка координат
coord* ClearCoords(coord* head); // Удаление списка координат
void PrintCoords(coord* head); // Вывод списка координат
coord* GetLast(coord* head);
coord* Parition(coord* first, coord* last);
void QuickSort(coord* first, coord* last);

int n = 0; //Количество целей

void readCoords(FILE * file_to_read, target* head); // Считывание списка целей из файла
void searchCircle(coord* head, target* headT, int rad); // Поиск окружности с оптимальными координатами
void searchCircleCenter(coord* coord_pair, int rad, double* xc1, double* yc1, double* xc2, double* yc2); // Поиск центра окружности
int checkCircle(target* headT, int rad, double* xc, double* yc); // Проверка всех точек на попадание в окружность
int searchForComma(char* str); // Поиск запятой в строке
double sqr(double x); // Квадрат числа

int main(int argc, char* argv[])
{
    char * file_name;
    char* rad_char;

    int rad;
    target* headT; // Вершина списка целей
    coord* headC; // Вершина списка координат

    // Считываем аттрибуты на входе программы
    if (argc != 3)
    {
        printf("Error: wrong arguments quantity.");
        return -1;
    }
    else
    {
        file_name = argv[1];
        rad_char = argv[2];
    }

    rad = atoi(rad_char);


    // Считываем данные из файла
    FILE* ftr;
    if ((ftr = fopen(file_name, "r")) ==  NULL) {
        printf("Error: Cannot open file \"%s\".", file_name);
        return -1;
    }
    headT = AddTarget(NULL); //Создание вершин без записи данных
    headC = AddCoord(NULL);
    readCoords(ftr, headT);
    fclose(ftr);

    if (n == 1) { // Если количество целей = 1
        printf("%d,%d 1", headT->x, headT->y);
        return 0;
    }

    CreateList(headC, headT); //Создаем список координат
    headC = DeleteCoordHead(headC); // Удаляем вершину списка, т.к. там мусор
    QuickSort(headC, GetLast(headC)); //Сортируем по возрастанию квадрата расстояния
    FilterCoord(headC, 2*rad); // Отбрасываем точки, расстояние между которыми больше диаметра

    searchCircle(headC, headT, rad); //Ищем оптимальную точку
}

target* AddTarget(target* head)
{
    target* tmp;
    tmp = head;
    if (head == NULL) {
        head = (target*)malloc(sizeof(target));
        head->next = NULL;
        return head;
    }
    while (tmp->next != NULL)
        tmp = tmp->next;
    
    tmp->next = (target*)malloc(sizeof(target));
    tmp->next->next = NULL;
    return tmp->next;
}
void DeleteTarget(target* target_to_delete, target* head)
{
    target* tmp;
    tmp = head;
    while (tmp->next != target_to_delete)
        tmp = tmp->next;
    tmp->next = (tmp->next)->next;
    free(target_to_delete);
}
void ClearTargets(target* head)
{
    while (head != NULL)
    {
        target* tmp = head;
        tmp = tmp->next;

        free(tmp);
    }
}
void PrintTargets(target* head)
{
    target* tmp = head;
    printf("Targets List:\n");
    while (tmp->next != NULL) {
        printf("%d, %d\n", tmp->x, tmp->y);
        tmp = tmp->next;
    }
}

coord* AddCoord(coord* head)
{
    coord* tmp;
    tmp = head;
    if (head == NULL) {
        head = (coord*)malloc(sizeof(coord));
        head->next = NULL;
        return head;
    }
    while (tmp->next != NULL)
        tmp = tmp->next;

    tmp->next = (coord*)malloc(sizeof(coord));
    tmp->next->next = NULL;
    return tmp->next;
}
coord* AddCoord(coord* head, target* tgf, target* tgs)
{
    coord* tmp;
    tmp = head;
    if (head == NULL) {
        head = (coord*)malloc(sizeof(coord));
        head->x0 = tgf->x;
        head->y0 = tgf->y;
        head->x = tgs->x;
        head->y = tgs->y;

        head->dist_sqr = sqr(head->x0 - head->x) + sqr(head->y0 - head->y);
        head->next = NULL;
        return head;
    }
    while (tmp->next != NULL)
        tmp = tmp->next;

    tmp->next = (coord*)malloc(sizeof(coord));
    tmp->next->x0 = tgf->x;
    tmp->next->y0 = tgf->y;
    tmp->next->x = tgs->x;
    tmp->next->y = tgs->y;
    tmp->next->dist_sqr = sqr(tgf->x - tgs->x) + sqr(tgf->y - tgs->y);
    tmp->next->next = NULL;
    return tmp->next;
}
void DeleteCoord(coord* coord_to_delete, coord* head)
{
    coord* temp;
    temp = head;

    if (coord_to_delete == head)
    {
        head = DeleteCoordHead(head);
        return;
    }
    while (temp->next != coord_to_delete)
        temp = temp->next;
    temp->next = (temp->next)->next;
    temp = coord_to_delete;
    free(temp);
}
coord* DeleteCoordHead(coord* head)
{
    coord* tmp;
    tmp = head;
    head = head->next;
    free(tmp);
    return head;
}
void FilterCoord(coord* head, int rad)
{
    coord* tmp = head;
    while (tmp != NULL) {
        if (tmp->next->dist_sqr > sqr(rad))
        {
            //DeleteCoord(tmp->next, head);
            tmp->next = ClearCoords(tmp->next);
            break;
        }
        else
            tmp = tmp->next;
    }
}
coord* ClearCoords(coord* head)
{
    coord* tmp;
    tmp = head;
    while (tmp)
    {  
        coord* to_del = tmp;
        tmp = tmp->next;
        free(to_del);
    }
    tmp = head;
    tmp = NULL;
    return NULL;
}
void PrintCoords(coord* head)
{
    coord* tmp = head;
    printf("Coords List:\n");
    while (tmp != NULL) {
        printf("%d, %d, %d, %d, %d\n", tmp->x0, tmp->y0, tmp->x, tmp->y, tmp->dist_sqr);
        tmp = tmp->next;
    }
}
void CreateList(coord* head, target* thead)
{
    target* tmpT1;
    target *tmpT2;
    coord* tmpC;

    tmpT1 = thead;
    tmpT2 = thead;

    tmpC = head;

    // Для каждой точки ищем расстояние между остальными точками и записываем в список координат

    while (tmpT1->next != NULL)
    {
        tmpT2 = tmpT1->next;
        while (tmpT2->next != NULL)
        {
            if (tmpT1 != tmpT2)
                tmpC = AddCoord(tmpC, tmpT1, tmpT2);
            tmpT2 = tmpT2->next;
        }
        tmpT1 = tmpT1->next;
    }
}
coord* GetLast(coord* head)
{
    coord* temp;
    coord* prelast;
    temp = head;
    while (temp->next != NULL)
    {
        prelast = temp;
        temp = temp->next;
    }
    //temp = temp->next;
    return temp;
}
coord* Parition(coord* first, coord* last)
{
    coord* pivot = first;
    coord* front = first;
    int tempds = 0; // Временная dist_sqr
    int tempx, tempy, tempx0, tempy0;
    while (front != NULL && front != last) {
        if (front->dist_sqr < last->dist_sqr) {
            pivot = first;

            // Меняем местами значения
            tempds = first->dist_sqr;
            tempx = first->x;
            tempx0 = first->x0;
            tempy = first->y;
            tempy0 = first->y0;

            first->dist_sqr = front->dist_sqr;
            first->x = front->x;
            first->x0 = front->x0;
            first->y = front->y;
            first->y0 = front->y0;

            front->dist_sqr = tempds;
            front->x = tempx;
            front->x0 = tempx0;
            front->y = tempy;
            front->y0 = tempy0;

            // Переходим к следующему элементу списка
            first = first->next;
        }

        // Переходим к следующему элементу списка
        front = front->next;
    }

    // Меняем местами данные последнего элемента с текущим
    tempds = first->dist_sqr;
    tempx = first->x;
    tempx0 = first->x0;
    tempy = first->y;
    tempy0 = first->y0;

    first->dist_sqr = last->dist_sqr;
    first->x = last->x;
    first->x0 = last->x0;
    first->y = last->y;
    first->y0 = last->y0;

    front->dist_sqr = tempds;
    front->x = tempx;
    front->x0 = tempx0;
    front->y = tempy;
    front->y0 = tempy0;

    last->dist_sqr = tempds;
    last->x = front->x;
    last->x0 = front->x0;
    last->y = front->y;
    last->y0 = front->y0;
    return pivot;
}
void QuickSort(coord* first, coord* last)
{
    if (first == last) {
        return;
    }
    coord* pivot = Parition(first, last);

    if (pivot != NULL && pivot->next != NULL) {
        QuickSort(pivot->next, last);
    }

    if (pivot != NULL && first != pivot) {
        QuickSort(first, pivot);
    }
}

void readCoords(FILE* file_to_read, target * head)
{
    char str[6]; // Строка, в которую считываем координаты из файла
    char first_coord[3] = "  "; // Координата х в виде строки
    char second_coord[3] = "  "; // Координата у в виде строки
    int i = 0; // Счетчики
    int j = 0;
    int o = 0;
    int comma = 0; // Индекс запятой в строке
    target* tg = head;
    while (!feof(file_to_read))
    {
        i = 0;
        while (i < 6) {
            str[i] = fgetc(file_to_read);
            if (str[i] == '\n' || str[i] == EOF)
                break;
            i++;
        }

        comma = searchForComma(str);

        for (j = 0; j < comma; j++)
            first_coord[j] = str[j];

        j += 1;
        o = 0;
        while (str[j] != '\n' && str[j] != EOF) {
            second_coord[o] = str[j];
            j++;
            o++;
        }

        tg->x = atoi(first_coord);
        tg->y = atoi(second_coord);
        
        if (tg->next == NULL)
            tg->next = AddTarget(tg);
        tg = tg->next;
        
        memset(str, ' ', 6);
        n += 1;
    }
}
int searchForComma(char* str)
{
    for (int i = 0; i < strlen(str); i++)
        if (str[i] == ',')
            return i;
}
double sqr(double x)
{
    return (x * x);
}

void searchCircle(coord* head, target* headT, int rad)
{
    coord* temp;
    temp = head;
    double xc1, yc1; //Центр первой окружности
    double xc2, yc2; //Центр второй окружности
    double xmax, ymax; //Центр оптимальной окружности
    int k, kmax; // Количество пораженных целей

    double* xp1;
    double* yp1;
    double* xp2;
    double* yp2;

    xp1 = &xc1;
    yp1 = &yc1;
    xp2 = &xc2;
    yp2 = &yc2;

    kmax = 0;
    xmax = 0;
    ymax = 0;
    while (temp != NULL)
    {
        searchCircleCenter(temp, rad, xp1, yp1, xp2, yp2);
        if (xc1 == xc2 && yc1 == yc2) //если точки совпадают
        {
            k = checkCircle(headT, rad, xp1, yp1);
            if (k > kmax) {
                kmax = k;
                xmax = *xp1;
                ymax = *yp1;
            }
        }
        else {
            k = checkCircle(headT, rad, xp1, yp1);
            if (k > kmax) {
                kmax = k;
                xmax = *xp1;
                ymax = *yp1;
            }
            k = checkCircle(headT, rad, xp2, yp2);
            if (k > kmax) {
                kmax = k;
                xmax = *xp2;
                ymax = *yp2;
            }
        }
        temp = temp->next;
    }

    if (xmax < 0)
        xmax = 0;
    if (xmax > 99)
        xmax = 99;
    if (ymax < 0)
        ymax = 0;
    if (ymax > 99)
        ymax = 99;
    printf("(%f, %f) %d\n", xmax, ymax, kmax);
}
void searchCircleCenter(coord* coord_pair, int rad, double* xc1, double* yc1, double* xc2, double* yc2)
{
    /*
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣤⠤⠔⠒⠒⠒⠉⠉⠉⠉⠉⠙⠒⠒⠒⠦⢤⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠤⠖⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠛⠢⢤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡴⠊⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠑⢤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠖⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠢⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠞⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠣⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡴⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⢦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠎⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢳⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠎⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢱⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡎⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡜⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀D(Dx; Dy)⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠘⠲⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠈⠳⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠈⠳⢄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢱⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀   ⣇   ⠀⠀⠀⠀⠑⢄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⡤⠶⠖⠒⠉⠉⠉⠉⠉⡏⠉⠉⠉⠙⠒⠲⠦⢄⣀⡑⢄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢢⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⠴⠒⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠒⢼⣢⡀⠀⠀⠀⠀⠀⠀⠀⢀⡜⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢣⠀⠀⠀⠀⠀⣠⠔⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢦⣄⠀⠀⠀⠀⢠⠜⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣄⠀⡤⠎⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇C(Cx; Cy)⠀⠀⠀⠀⠀⠀⠉⠣⣄⠀⡰⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀A(Ax; Ay)⠀⠀⣨⠿⣒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⡗⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⠒⣺⠯⣅⠀⠀B(Bx; By)⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠜⠁⠀⠉⠳⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡴⠊⠁⠀⠘⢧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠊⠀⠀⠀⠀⠀⠀⠑⢢⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⡴⠋⠀⠀⠀⠀⠀⠀⢱⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠓⢤⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⢀⣤⠴⢚⠝⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⢱⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠑⠒⠤⠤⠤⠤⠤⡧⠤⠤⠤⠴⠒⠊⠉⠁⣀⠔⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡜⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⡠⠎⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⢀⡠⠚⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⢀⡰⠊⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣇⠔⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁E⠀(Ex; Ey)⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢣⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢣⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠞⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢳⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠚⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡴⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠱⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⢄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡤⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠲⢄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡠⠖⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠓⠢⢄⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⡤⠖⠊⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠑⠒⠒⠒⠤⠤⠤⠤⠤⠴⠒⠒⠒⠋⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    Через 2 точки (А и В) можно провести две окружности так, что А и В образуют хорду
    и будут удалены от центра окружностей на R оденственным образом.
    Обозначим центры окружностей как D и Е.
    */
    double ax, ay;
    double bx, by;
    double dx, dy;
    double ex, ey;
    double ab, dc;

    ax = coord_pair->x0;
    ay = coord_pair->y0;
    bx = coord_pair->x;
    by = coord_pair->y;

    ab = sqrt(coord_pair->dist_sqr); //Длина АВ
    if (ab == 0) // Если длина АВ == 0 (точки совпадают)
    {
        dx = ax;
        dy = ay;
        ex = ax;
        ey = ay;
    }
    else
    {
        dc = sqr(rad) - sqr(ab);
        dc = abs((long)dc);
        dc = sqrt(dc);

        dx = bx + ((ax - bx) / 2) + (dc * (ay - by) / ab);
        dy = by + ((ay - by) / 2) - (dc * (ax - bx) / ab);

        ex = bx + ((ax - bx) / 2) - (dc * (ay - by) / ab);
        ey = by + ((ay - by) / 2) + (dc * (ax - bx) / ab);
    }
    *xc1 = dx;
    *yc1 = dy;

    *xc2 = ex;
    *yc2 = ey;
}
int checkCircle(target* headT, int rad, double* xc, double* yc)
{
    /* Уравнение окружности
    * (x - xc)^2 + (y - yc)^2 = R^2
    * 
    * x, y - координаты проверяемой точки
    * xc, yc - центр окружности
    * R - Квадрат радиуса
    */
    target* temp;
    temp = headT;
    double local_x, local_y; // Координаты проверяемых точек
    double xcc, ycc; // Центр окружности
    double sqr_rad; // Расстояние от центра окружности до точки
    // Если точка попадает в окружность, то sqr_rad <= R^2
    int counter; // Счетчик целей внутри окружности

    counter = 0;

    xcc = *xc;
    ycc = *yc;

    while (temp->next != NULL)
    {
        local_x = temp->x;
        local_y = temp->y;

        sqr_rad = sqr(local_x - xcc) + sqr(local_y - ycc);
        if (sqr_rad <= sqr(rad))
            counter++;
        temp = temp->next;
    }

    return counter;
}