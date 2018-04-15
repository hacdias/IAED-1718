#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(X,Y) (((X) < (Y)) ? (X) : (Y))
#define max(X,Y) (((X) > (Y)) ? (X) : (Y))
#define MAX 10000
#define MAX_COMMAND 100
#define MAX_FILENAME 300

typedef struct {
  unsigned int line;
  unsigned int column;
  double value;
} Point;

typedef struct {
  unsigned int total;
  unsigned int mini, minj, maxi, maxj;
  Point point[MAX];
} Matrix;

int readLine (char *s, int n, FILE *stream);
void initializeMatrix (Matrix *m);
void addPoint (Matrix *m, unsigned int line, unsigned int col, double val);
void removePoint (Matrix *m, int line, int column);
void removePointByValue (Matrix *m, double value);
void printPoints (Matrix *m);
void matrixInfo (Matrix *m);
void compress(Matrix *m);

void atualizaCaracteristicaPonto (Matrix *m, Point *p);
void atualizaCaracteristica (Matrix *m);
void printLine (Matrix *m, unsigned int line);
void printColumn (Matrix *m, unsigned int col);

void sort (Matrix *m, int byColumn);
int sortByLine (const void * a, const void * b);
int sortByColumn (const void * a, const void * b);

void loadFromFile (Matrix *m, const char *filename);
void saveToFile (Matrix *m, const char *filename);

int main(int argc, char ** argv) {
  char cmd[MAX_COMMAND + 1];
  char filename[MAX_FILENAME + 1];

  int len = 0, a, b;
  double c;
  
  Matrix matriz;
  initializeMatrix(&matriz);

  if (argc == 2) {
    strcpy(filename, argv[1]);
    loadFromFile(&matriz, filename);
  }

  while ((len = readLine(cmd, MAX_COMMAND, stdin))) {
    switch (cmd[0]) {
      case 'a':
        sscanf(cmd, "a %d %d %lf", &a, &b, &c);
        addPoint(&matriz, a, b, c);
        break;
      case 'p':
        printPoints(&matriz);
        break;
      case 'i':
        matrixInfo(&matriz);
        break;
      case 'l':
        sscanf(cmd, "l %d", &a);
        printLine(&matriz, a);
        break;
      case 'c':
        sscanf(cmd, "c %d", &a);
        printColumn(&matriz, a);
        break;
      case 'o':
        sort(&matriz, len != 1);
        break;
      case 'z':
        sscanf(cmd, "z %lf", &c);
        removePointByValue(&matriz, c);
        break;
      case 's':
        compress(&matriz);
        break;
      case 'w':
        if (len != 1) {
          sscanf(cmd, "w %s", filename);
        }
        saveToFile(&matriz, filename);
        break;
      case 'q':
        return 0;
    }
  }

  return 0;
}

int readLine (char *s, int n, FILE *stream) {
  char c;
  int i = 0;
  while ((c = fgetc(stream)) != '\n' && i < n && c != EOF) {
    s[i] = c;
    i++;
  }
  s[i] = '\0';
  return i;
}

void loadFromFile (Matrix *m, const char *filename) {
  FILE *file = fopen(filename, "r");
  char s[MAX_COMMAND + 1];
  unsigned int l, c;
  double v;

  while (readLine(s, MAX_COMMAND, file)) {
    sscanf(s, "%d %d %lf", &l, &c, &v);
    addPoint(m, l, c, v);
  }

  fclose(file);
}

void saveToFile (Matrix *m, const char *filename) {
  FILE *file = fopen(filename, "w");

  for (int i = 0; i < m->total; i++) {
    fprintf(file, "%d %d %lf\n", m->point[i].line, m->point[i].column, m->point[i].value);
  }

  fclose(file);
}

void initializeMatrix (Matrix *m) {
  m->total = 0;
  m->mini = MAX;
  m->maxi = 0;
  m->minj = MAX;
  m->maxj = 0;
}

Point * findPoint (Matrix *m, int line, int column) {
  for (int i = 0; i < m->total; i++)
    if (m->point[i].line == line && m->point[i].column == column)
      return &m->point[i];

  return NULL;
}

void addPoint (Matrix *m, unsigned int line, unsigned int col, double val) {
  // Removes the element.
  if (val == 0) {
    return removePoint(m, line, col);
  }

  // Checks if the element already exists.
  Point *p;
  if ((p = findPoint(m, line, col)) != NULL) {
    p->value = val;
    return;
  }

  // Adds the point.
  m->point[m->total].line = line;
  m->point[m->total].column = col;
  m->point[m->total].value = val;
  atualizaCaracteristicaPonto(m, &m->point[m->total]);
  m->total++;
}

void printPoints (Matrix *m) {
  if (m->total == 0) {
    printf("empty matriz\n");
    return;
  }

  for (int i = 0; i < m->total; i++) {
    printf("[%d;%d]=%.3lf\n",
      m->point[i].line,
      m->point[i].column,
      m->point[i].value);
  }
}

int matrixDimension (Matrix *m) {
  return (m->maxj - m->minj + 1) * (m->maxi - m->mini + 1);
}

double matrixDensity (Matrix *m) {
  return (m->total / (double) matrixDimension(m)) * 100;
}

void matrixInfo (Matrix *m) {
  int dim = matrixDimension(m);
  double dens = matrixDensity(m);

  printf("[%d %d] [%d %d] %d / %d = %.3lf%%\n",
    m->mini,
    m->minj,
    m->maxi,
    m->maxj,
    m->total,
    dim,
    dens);
}

void removePoint (Matrix *m, int line, int column) {
  int i;

  for (i = 0; (m->point[i].line != line || m->point[i].column != column); i++);
  for (i = i + 1; i < m->total; i++) {
    m->point[i - 1] = m->point[i];
  }

  m->total--;
  atualizaCaracteristica(m);
}

void removePointByValue (Matrix *m, double value) {
  int i = 0, j = 0;

  while (i < m->total) {
    if (m->point[j].value == value) {
      j++;
      m->total--;
    } else {
      m->point[i] = m->point[j];
      i++;
      j++;
    }
  }

  atualizaCaracteristica(m);
}

void atualizaCaracteristicaPonto (Matrix *m, Point *p) {
  m->mini = min(m->mini, p->line);
  m->minj = min(m->minj, p->column);
  m->maxi = max(m->maxi, p->line);
  m->maxj = max(m->maxj, p->column);
}

void atualizaCaracteristica (Matrix *m) {
  for (int i = 0; i < m->total; i++) {
    atualizaCaracteristicaPonto(m, &m->point[i]);
  }
}

void printLine (Matrix *m, unsigned int line) {
  int dim = m->maxj - m->minj + 1, i, empty = 1;
  double values[dim];

  if (line > m->maxi || line < m->mini) {
    printf("empty line\n");
    return;
  }

  for (i = 0; i < dim; i++) values[i] = 0;
  for (i = 0; i < m->total; i++) {
    if (m->point[i].line == line) {
      values[m->point[i].column - m->minj] = m->point[i].value;
      empty = 0;
    }
  }

  if (empty) {
    printf("empty line\n");
    return;
  }

  for(i = 0; i < dim; i++) printf(" %.3lf", values[i]);
  printf("\n");
}

void printColumn (Matrix *m, unsigned int col) {
  int dim = m->maxi - m->mini + 1, i, empty = 1;
  double values[dim];

  if (col > m->maxj || col < m->minj) {
    printf("empty column\n");
    return;
  }

  for (i = 0; i < dim; i++) values[i] = 0;
  for (i = 0; i < m->total; i++) {
    if (m->point[i].column == col) {
      values[m->point[i].line - m->mini] = m->point[i].value;
      empty = 0;
    }
  }

  if (empty) {
    printf("empty column\n");
    return;
  }

  for(i = 0; i < dim; i++) {
    printf("[%d;%d]=%.3lf\n", i, col, values[i]);
  }
}

int sortByColumn (const void * a, const void * b) {
  const Point *p1 = (Point *)a;
  const Point *p2 = (Point *)b;

  if (p1->column < p2->column) {
    return -1;
  } else if (p1->column > p2->column) {
    return +1;
  } else if (p1->column == p2->column) {
    return sortByLine(a, b);
  }

  return 0;
}

int sortByLine (const void * a, const void * b) {
  const Point *p1 = (Point *)a;
  const Point *p2 = (Point *)b;

  if (p1->line < p2->line) {
    return -1;
  } else if (p1->line > p2->line) {
    return +1;
  } else if (p1->line == p2->line) {
    return sortByColumn(a, b);
  }

  return 0;
}

void sort (Matrix *m, int byColumn) {
  if (byColumn) {
    qsort(m->point, m->total, sizeof(Point), sortByColumn);
  } else {
    qsort(m->point, m->total, sizeof(Point), sortByLine);
  }
}

void getLine (double **storage, Matrix *m, unsigned int line) {
  for (int i = 0; i < m->total; i++) {
    if (m->point[i].line == line) {
      storage[m->point[i].column - m->minj] = &m->point[i].value;
    }
  }
}

int calculateOffset (double **values, int n) {
  for (int i = 0; i < n; i++)
    if (*values[n] != 0)
      return i;

  return -1;
}

void compress (Matrix *m) {
  if (matrixDensity(m) > 50) {
    printf("dense matrix\n");
    return;
  }
  
  double value[m->total];
  int height = m->maxi - m->mini + 1,
    lineCount = 0, i, j,
    line, density,
    index[m->total],
    densities[height];

  for (i = 0; i < height; i++) densities[i] = 0;
  for (i = 0; i < m->total; i++) densities[m->point[i].line - m->mini]++;
  for (i = 0; i < height; i++) if (densities[i] != 0) lineCount++;

  int columnCount = m->maxj - m->minj + 1;

  double zero = 0;
  double * values[columnCount];

  int offsets[lineCount];

  for (i = 0; i < lineCount; i++) {
    for (j = 0; j < columnCount; j++) values[j] = &zero;
    line = 0;
    density = 0;

    for (j = 0; j < height; j++) {
      if (densities[j] == density) {
        line = min(line, j);
      } else if (densities[j] > density) {
        line = j;
        density = densities[j];
      }      
    }

    getLine(values, m, line);
    offsets[i] = calculateOffset(values, line);

    densities[line] = 0;
    line += m->mini;

    printf("Line %d\n", line);  
  }

  printf("dens = ");
  for (int i = 0; i < height; i++) printf("%d ", densities[i]);
}
