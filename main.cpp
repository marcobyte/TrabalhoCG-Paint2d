#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    float x;
    float y;
} Point;


typedef struct {
    Point start;
    Point end1;
} Line;

#define MAX_VERTICES 10
typedef struct {
    int numVertices;
    Point vertices[MAX_VERTICES];
} Poligono;

typedef struct {
    int type;
    union {
        Point point;
        Line line;
        Poligono polygon;
    } data;
} Object;

Object objects[100];
int objectCount = 0;
int selectedObject = -1;
int mouseX, mouseY;
int linha = 0;
int verticesToCapture = 0;
int objetoSelecionado = -1;

void moveObjectMouseCallback(int,int,int,int);
void createSelectMenu();
void rotateObject(int,int);
void scaleObject(int,int);
void shearXObject(int,int);
void reflectObject(int,int);
void deletarObjeto();

void drawObjects() {
    for (int i = 0; i < objectCount; i++) {
        if (objects[i].type == 0) {
            glBegin(GL_POINTS);
            glVertex2f(objects[i].data.point.x, objects[i].data.point.y);
            glEnd();
        } else if (objects[i].type == 1) {
            glBegin(GL_LINES);
            glVertex2f(objects[i].data.line.start.x, objects[i].data.line.start.y);
            glVertex2f(objects[i].data.line.end1.x, objects[i].data.line.end1.y);
            glEnd();
        } else if (objects[i].type == 2) {
            glBegin(GL_POLYGON);
            for (int j = 0; j < objects[i].data.polygon.numVertices; j++) {
                glVertex2f(objects[i].data.polygon.vertices[j].x, objects[i].data.polygon.vertices[j].y);
            }
            glEnd();
        }
    }
}

void drawObject(int index) {
    switch (objects[index].type) {
        case 0:
            glBegin(GL_POINTS);
            glColor3f(1.0, 1.0, 0.0);
            glVertex2f(objects[index].data.point.x, objects[index].data.point.y);
            glEnd();
            break;
        case 1:
            glBegin(GL_LINES);
            glVertex2f(objects[index].data.line.start.x, objects[index].data.line.start.y);
            glVertex2f(objects[index].data.line.end1.x, objects[index].data.line.end1.y);
            glEnd();
            break;
        case 2:
            glBegin(GL_POLYGON);
            for (int j = 0; j < objects[index].data.polygon.numVertices; j++) {
                glVertex2f(objects[index].data.polygon.vertices[j].x, objects[index].data.polygon.vertices[j].y);
            }
            glEnd();
            break;
    }
}

void mouseCallback(int button, int state, int x, int y) {
    mouseX = x;
    mouseY = glutGet(GLUT_WINDOW_HEIGHT) - y;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (objectCount < 100) {
            switch (selectedObject) {
                case 0:
                    objects[objectCount].type = 0;
                    objects[objectCount].data.point.x = (float)mouseX;
                    objects[objectCount].data.point.y = (float)mouseY;
                    objectCount++;
                    break;
                case 1:
                    if (linha == 0) {
                        objects[objectCount].type = 1;
                        objects[objectCount].data.line.start.x = (float)mouseX;
                        objects[objectCount].data.line.start.y = (float)mouseY;
                        linha = 1;
                    } else if(linha == 1){
                        objects[objectCount].data.line.end1.x = (float)mouseX;
                        objects[objectCount].data.line.end1.y = (float)mouseY;
                        objectCount++;
                        linha = 0;
                    }
                    break;
                case 2:
                    if (verticesToCapture > 0) {
                        if (objects[objectCount].type != 2) {
                            objects[objectCount].type = 2;
                            objects[objectCount].data.polygon.numVertices = 0;
                        }

                        objects[objectCount].data.polygon.vertices[objects[objectCount].data.polygon.numVertices].x = (float)mouseX;
                        objects[objectCount].data.polygon.vertices[objects[objectCount].data.polygon.numVertices].y = (float)mouseY;
                        objects[objectCount].data.polygon.numVertices++;

                        verticesToCapture--;

                        if (verticesToCapture == 0) {
                            objectCount++;
                        }
                    } else {
                        printf("Digite o numero de vertices do poligono: ");
                        scanf("%d", &verticesToCapture);

                        if (verticesToCapture <= 0 || verticesToCapture > MAX_VERTICES) {
                            printf("Numero invalido. Definindo para 0.\n");
                            verticesToCapture = 0;
                        }
                    }
                    break;
            }
            glutPostRedisplay();
        }
    }
}


void selecao () {
    int index;
    printf("Escolha um objeto: ");
    scanf("%d", &index);
    objetoSelecionado = index-1;
}

void menuCallback(int value) {
    selectedObject = value;

    if (selectedObject == 3) {
        selecao();
        printf("Objeto selecionado. Escolha a acao.\n");
        createSelectMenu();  // Cria o submenu para ações de seleção
    }
}

void createMenu() {
    // Criação do menu e das entradas
    int menu = glutCreateMenu(menuCallback);
    glutAddMenuEntry("Ponto", 0);
    glutAddMenuEntry("Linha", 1);
    glutAddMenuEntry("Poligono", 2);
    glutAddMenuEntry("Selecionar", 3);

    // Associar o menu ao botão direito do mouse
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

typedef struct {
    float m[3][3];
} Matrix3x3;

Matrix3x3 matrix_translacao(float tx, float ty) {
    Matrix3x3 mat = {{{1, 0, tx},
                      {0, 1, ty},
                      {0, 0, 1}}};
    return mat;
}

Matrix3x3 matrix_rotacao(float theta) {
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);
    Matrix3x3 mat = {{{cosTheta, -sinTheta, 0},
                      {sinTheta, cosTheta, 0},
                      {0, 0, 1}}};
    return mat;
}

Matrix3x3 matrix_escala(float sx, float sy) {
    Matrix3x3 mat = {{{sx, 0, 0},
                      {0, sy, 0},
                      {0, 0, 1}}};
    return mat;
}

Matrix3x3 matrix_cisalhamento_x(float shx) {
    Matrix3x3 mat = {{{1, shx, 0},
                      {0, 1, 0},
                      {0, 0, 1}}};
    return mat;
}

Point transformPoint(Point p, Matrix3x3 mat) {
    float homogeneousPoint[3] = {p.x, p.y, 1};
    float result[3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i] += mat.m[i][j] * homogeneousPoint[j];
        }
    }
    Point transformedPoint = {result[0], result[1]};
    return transformedPoint;
}

Line translateLine(Line linha, float tx, float ty) {
    Matrix3x3 mat = matrix_translacao(tx,ty);
    Line nova_linha;
    nova_linha.start = transformPoint(linha.start, mat);
    nova_linha.end1 = transformPoint(linha.end1, mat);
    return nova_linha;
}

Poligono translatePolygon(Poligono polygon, float tx, float ty) {
    Matrix3x3 mat = matrix_translacao(tx,ty);
    Poligono novo_poligono;
    for (int i = 0; i < polygon.numVertices; i++) {
        novo_poligono.vertices[i] = transformPoint(polygon.vertices[i], mat);
    }
    return novo_poligono;
}

void moveObject() {
    if (objetoSelecionado != -1 && objetoSelecionado < objectCount) {
        printf("Clique com o mouse na nova posicao para onde o objeto sera movido.\n");
        glutMouseFunc(moveObjectMouseCallback);
    } else {
        printf("Nenhum objeto selecionado.\n");
    }
}

void moveObjectMouseCallback(int button, int state, int x, int y) {
    mouseX = x;
    mouseY = glutGet(GLUT_WINDOW_HEIGHT) - y;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Atualiza a posição do objeto movido
        Point newLocation = {(float)mouseX, (float)mouseY};

        switch (objects[objetoSelecionado].type) {
            case 0:  // Ponto
                objects[objetoSelecionado].data.point = newLocation;
                break;
            case 1:  // Linha
                objects[objetoSelecionado].data.line = translateLine(objects[objetoSelecionado].data.line, newLocation.x - objects[objetoSelecionado].data.line.start.x, newLocation.y - objects[objetoSelecionado].data.line.start.y);
                break;
            case 2:  // Polígono
                float deltaX = newLocation.x - objects[objetoSelecionado].data.polygon.vertices[0].x;
                float deltaY = newLocation.y - objects[objetoSelecionado].data.polygon.vertices[0].y;

                for (int i = 0; i < objects[objetoSelecionado].data.polygon.numVertices; i++) {
                    objects[objetoSelecionado].data.polygon.vertices[i].x += deltaX;
                    objects[objetoSelecionado].data.polygon.vertices[i].y += deltaY;
                }
        }
        glutMouseFunc(mouseCallback);
        glutPostRedisplay();
    }
}

Line rotateLine(Line line, float theta) {
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    Point midPoint = {(line.start.x + line.end1.x) / 2, (line.start.y + line.end1.y) / 2};
    line.start.x -= midPoint.x;
    line.start.y -= midPoint.y;
    line.end1.x -= midPoint.x;
    line.end1.y -= midPoint.y;

    // Aplicar a rotação
    float xTemp, yTemp;
    xTemp = line.start.x * cosTheta - line.start.y * sinTheta;
    yTemp = line.start.x * sinTheta + line.start.y * cosTheta;
    line.start.x = xTemp;
    line.start.y = yTemp;

    xTemp = line.end1.x * cosTheta - line.end1.y * sinTheta;
    yTemp = line.end1.x * sinTheta + line.end1.y * cosTheta;
    line.end1.x = xTemp;
    line.end1.y = yTemp;

    // Transladar de volta para a posição original
    line.start.x += midPoint.x;
    line.start.y += midPoint.y;
    line.end1.x += midPoint.x;
    line.end1.y += midPoint.y;

    return line;
}

Poligono rotatePolygon(Poligono polygon, float theta) {
    float centerX = 0.0, centerY = 0.0;

    for (int i = 0; i < polygon.numVertices; i++) {
        centerX += polygon.vertices[i].x;
        centerY += polygon.vertices[i].y;
    }
    centerX /= polygon.numVertices;
    centerY /= polygon.numVertices;

    for (int i = 0; i < polygon.numVertices; i++) {
        polygon.vertices[i].x -= centerX;
        polygon.vertices[i].y -= centerY;
    }
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    // Aplicar a rotação
    for (int i = 0; i < polygon.numVertices; i++) {
        float xTemp = polygon.vertices[i].x * cosTheta - polygon.vertices[i].y * sinTheta;
        float yTemp = polygon.vertices[i].x * sinTheta + polygon.vertices[i].y * cosTheta;
        polygon.vertices[i].x = xTemp;
        polygon.vertices[i].y = yTemp;
    }

    // Transladar de volta para a posição original
    for (int i = 0; i < polygon.numVertices; i++) {
        polygon.vertices[i].x += centerX;
        polygon.vertices[i].y += centerY;
    }

    return polygon;
}

Line scaleLine(Line line, float sx, float sy) {
    if (sx != 0 && sy != 0) {
        Point midPoint = {(line.start.x + line.end1.x) / 2, (line.start.y + line.end1.y) / 2};

        line.start.x -= midPoint.x;
        line.start.y -= midPoint.y;
        line.end1.x -= midPoint.x;
        line.end1.y -= midPoint.y;

        // Aplicar a escala
        Matrix3x3 mat = matrix_escala(sx, sy);
        line.start = transformPoint(line.start, mat);
        line.end1 = transformPoint(line.end1, mat);

        // Transladar de volta para a posição original
        line.start.x += midPoint.x;
        line.start.y += midPoint.y;
        line.end1.x += midPoint.x;
        line.end1.y += midPoint.y;
    } else {
        printf("Fatores de escala inválidos (sx ou sy igual a zero).\n");
    }
    return line;
}

Poligono scalePolygon(Poligono polygon, float sx, float sy) {
    if (sx != 0 && sy != 0) {
        float centerX = 0.0, centerY = 0.0;

        for (int i = 0; i < polygon.numVertices; i++) {
            centerX += polygon.vertices[i].x;
            centerY += polygon.vertices[i].y;
        }
        centerX /= polygon.numVertices;
        centerY /= polygon.numVertices;

        for (int i = 0; i < polygon.numVertices; i++) {
            polygon.vertices[i].x -= centerX;
            polygon.vertices[i].y -= centerY;
        }

        // Aplicar a escala
        Matrix3x3 mat = matrix_escala(sx, sy);
        for (int i = 0; i < polygon.numVertices; i++) {
            polygon.vertices[i] = transformPoint(polygon.vertices[i], mat);
        }
        // Transladar de volta para a posição original
        for (int i = 0; i < polygon.numVertices; i++) {
            polygon.vertices[i].x += centerX;
            polygon.vertices[i].y += centerY;
        }
    } else {
        printf("Fatores de escala inválidos .\n");
    }
    return polygon;
}

Line shearXLine(Line line, float shx) {
    if (shx != 0) {
        Point midPoint = {(line.start.x + line.end1.x) / 2, (line.start.y + line.end1.y) / 2};

        line.start.x -= midPoint.x;
        line.start.y -= midPoint.y;
        line.end1.x -= midPoint.x;
        line.end1.y -= midPoint.y;

        // Aplicar o cisalhamento
        Matrix3x3 mat = matrix_cisalhamento_x(shx);
        line.start = transformPoint(line.start, mat);
        line.end1 = transformPoint(line.end1, mat);

        // Transladar de volta para a posição original
        line.start.x += midPoint.x;
        line.start.y += midPoint.y;
        line.end1.x += midPoint.x;
        line.end1.y += midPoint.y;
    } else {
        printf("Fator de cisalhamento invalido.\n");
    }
    return line;
}

Poligono shearXPolygon(Poligono polygon, float shx) {
    if (shx != 0) {
        float centerX = 0.0, centerY = 0.0;

        for (int i = 0; i < polygon.numVertices; i++) {
            centerX += polygon.vertices[i].x;
            centerY += polygon.vertices[i].y;
        }
        centerX /= polygon.numVertices;
        centerY /= polygon.numVertices;

        for (int i = 0; i < polygon.numVertices; i++) {
            polygon.vertices[i].x -= centerX;
            polygon.vertices[i].y -= centerY;
        }

        // Aplicar o cisalhamento
        Matrix3x3 mat = matrix_cisalhamento_x(shx);
        for (int i = 0; i < polygon.numVertices; i++) {
            polygon.vertices[i] = transformPoint(polygon.vertices[i], mat);
        }

        // Transladar de volta para a posição original
        for (int i = 0; i < polygon.numVertices; i++) {
            polygon.vertices[i].x += centerX;
            polygon.vertices[i].y += centerY;
        }
    } else {
        printf("Fator de cisalhamento inválido.\n");
    }

    return polygon;
}

Line reflectOriginLine(Line line) {

    line.start.x = -line.start.x;
    line.start.y = -line.start.y;
    line.end1.x = -line.end1.x;
    line.end1.y = -line.end1.y;

    return line;
}

Poligono reflectOriginPolygon(Poligono polygon) {
    float centerX = 0.0, centerY = 0.0;

        for (int i = 0; i < polygon.numVertices; i++) {
            centerX += polygon.vertices[i].x;
            centerY += polygon.vertices[i].y;
        }

        centerX /= polygon.numVertices;
        centerY /= polygon.numVertices;

        for (int i = 0; i < polygon.numVertices; i++) {
            polygon.vertices[i].x -= centerX;
            polygon.vertices[i].y -= centerY;
        }

        //refletir
        for (int i = 0; i < polygon.numVertices; i++) {
            polygon.vertices[i].x = -polygon.vertices[i].x;
            polygon.vertices[i].y = -polygon.vertices[i].y;
        }
        for (int i = 0; i < polygon.numVertices; i++) {
            polygon.vertices[i].x += centerX;
            polygon.vertices[i].y += centerY;
        }
    return polygon;
}

void selectMenuCallback(int value) {
    // Seleção
    switch (value) {
        case 0:
            printf("Movendo objeto...\n");
            moveObject();
            break;
        case 1:
            printf("Rotacionando objeto...\n");
            rotateObject(objetoSelecionado, objects[objetoSelecionado].type);
            break;
        case 2:
            printf("Escalando objeto...\n");
            scaleObject(objetoSelecionado, objects[objetoSelecionado].type);
            break;

        case 3:
            printf("Cisalhando objeto...\n");
            shearXObject(objetoSelecionado, objects[objetoSelecionado].type);
            break;
        case 4:
            printf("Refletindo objeto...\n");
            reflectObject(objetoSelecionado, objects[objetoSelecionado].type);
            break;
        case 5:
            printf("Deletando objeto...\n");
            deletarObjeto();
            break;
        case 6:
            printf("Cancelado.\n");
            createMenu();
            break;
    }
}

void rotateObject(int index, int objectType) {
    if (index != -1 && index < objectCount) {
        float theta;
        printf("Digite o angulo de rotacao (em graus): ");
        scanf("%f", &theta);

        switch (objectType) {
            case 0:
                printf("Operacao indisponivel para pontos.");
                break;
            case 1:  // Linha
                objects[index].data.line = rotateLine(objects[index].data.line, theta);
                break;
            case 2:  // Polígono
                objects[index].data.polygon = rotatePolygon(objects[index].data.polygon, theta);
                break;
            // Adicione casos para outros tipos de objetos, se necessário
        }
        glutPostRedisplay();
    }
}

void scaleObject(int index, int objectType) {
    if (index != -1 && index < objectCount) {
        float sx, sy;
        printf("Digite os fatores de escala (sx sy): ");
        scanf("%f %f", &sx, &sy);

        switch (objectType) {
            case 0: // Ponto
                printf("Oporacao indisponivel para pontos.");
                break;
            case 1:  // Linha
                objects[index].data.line = scaleLine(objects[index].data.line, sx, sy);
                break;
            case 2:  // Polígono
                objects[index].data.polygon = scalePolygon(objects[index].data.polygon, sx, sy);
                break;
        }
        glutPostRedisplay();
    }
}

void shearXObject(int index, int objectType) {
    if (index != -1 && index < objectCount) {
        float shx;
        printf("Digite o fator de cisalhamento em relação ao eixo x: ");
        scanf("%f", &shx);

        switch (objectType) {
            case 0:
                printf("Operacao indisponivel para pontos.");
                break;
            case 1:  // Linha
                objects[index].data.line = shearXLine(objects[index].data.line, shx);
                break;
            case 2:  // Polígono
                objects[index].data.polygon = shearXPolygon(objects[index].data.polygon, shx);
                break;
        }
        glutPostRedisplay();
    }
}

void reflectObject(int index, int objectType) {
    if (index != -1 && index < objectCount) {
        switch (objectType) {
            case 0:
                printf("Operacao indisponivel para pontos.\n");
                break;
            case 1:  // Linha
                objects[index].data.line = reflectOriginLine(objects[index].data.line);
                break;
            case 2:  // Polígono
                objects[index].data.polygon = reflectOriginPolygon(objects[index].data.polygon);
                break;

        }
        glutPostRedisplay();
    }
}


void createSelectMenu() {
    // Criação do submenu para seleção
    int selectMenu = glutCreateMenu(selectMenuCallback);
    glutAddMenuEntry("Transladar", 0);
    glutAddMenuEntry("Rotacionar", 1);
    glutAddMenuEntry("Escalar", 2);
    glutAddMenuEntry("Cisalhar", 3);
    glutAddMenuEntry("Refletir", 4);
    glutAddMenuEntry("Deletar", 5);
    glutAddMenuEntry("Cancelar", 6);

    // Associar o submenu ao botão direito do mouse
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void printSelectedObject() {
    printf("Selected Object: %d\n", selectedObject);
}

void printAllObjects() {
    printf("Objects:\n");
    for (int i = 0; i < objectCount; i++) {
        printf("Object %d - Type: %d", i + 1, objects[i].type);

        if (objects[i].type == 0) {  // Ponto
            printf(", Coordinates: (%f, %f)\n", objects[i].data.point.x, objects[i].data.point.y);
        } else if (objects[i].type == 1) {  // Linha
            printf(", Start: (%f, %f), End: (%f, %f)\n", objects[i].data.line.start.x, objects[i].data.line.start.y,
                                                         objects[i].data.line.end1.x, objects[i].data.line.end1.y);  // Alteração aqui de "end" para "end1"
        } else if (objects[i].type == 2) {  // Polígono
            printf(", NumVertices: %d, Vertices: ", objects[i].data.polygon.numVertices);
            for (int j = 0; j < objects[i].data.polygon.numVertices; j++) {
                printf("(%f, %f) ", objects[i].data.polygon.vertices[j].x, objects[i].data.polygon.vertices[j].y);
            }
            printf("\n");
        }
    }
}

void deletarObjeto () {
    printf("Digite o numero do objeto a ser deletado: ");
    selecao();

    for(int i = objetoSelecionado; i < objectCount; i++){
        switch(objects[i+1].type){
            case 0:  // Ponto
                objects[i].type = objects[i+1].type;
                objects[i].data.point.x = objects[i+1].data.point.x;
                objects[i].data.point.y = objects[i+1].data.point.y;
                break;
            case 1:  // Linha
                objects[i].type = objects[i+1].type;
                objects[i].data.line.start.x = objects[i+1].data.line.start.x;
                objects[i].data.line.start.y = objects[i+1].data.line.start.y;

                objects[i].data.line.end1.x = objects[i+1].data.line.end1.x;
                objects[i].data.line.end1.y = objects[i+1].data.line.end1.y;
                break;
            case 2:
                objects[i].type = objects[i+1].type;
                objects[i].data.polygon.numVertices = objects[i+1].data.polygon.numVertices;

                for(int j = 0; j < objects[i+1].data.polygon.numVertices; j++){
                    objects[i].data.polygon.vertices[j].x = objects[i+1].data.polygon.vertices[j].x;
                    objects[i].data.polygon.vertices[j].y = objects[i+1].data.polygon.vertices[j].y;
                }
                break;
        }
    }
    glutPostRedisplay();
    objectCount --;
}

void saveToFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo para salvar.\n");
        return;
    }

    // Escrever os objetos no arquivo
    for (int i = 0; i < objectCount; i++) {
        fprintf(file, "%d ", objects[i].type);

        if (objects[i].type == 0) {
            fprintf(file, "%f %f", objects[i].data.point.x, objects[i].data.point.y);
        } else if (objects[i].type == 1) {
            fprintf(file, "%f %f %f %f", objects[i].data.line.start.x, objects[i].data.line.start.y,
                                          objects[i].data.line.end1.x, objects[i].data.line.end1.y);
        } else if (objects[i].type == 2) {
            fprintf(file, "%d", objects[i].data.polygon.numVertices);
            for (int j = 0; j < objects[i].data.polygon.numVertices; j++) {
                fprintf(file, " %f %f", objects[i].data.polygon.vertices[j].x, objects[i].data.polygon.vertices[j].y);
            }
        }

        fprintf(file, "\n");
    }

    fclose(file);
}

void loadFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo para carregar.\n");
        return;
    }

    objectCount = 0;
    selectedObject = -1;

    while (fscanf(file, "%d", &objects[objectCount].type) != EOF) {
        if (objects[objectCount].type == 0) {
            fscanf(file, "%f %f", &objects[objectCount].data.point.x, &objects[objectCount].data.point.y);
        } else if (objects[objectCount].type == 1) {
            fscanf(file, "%f %f %f %f", &objects[objectCount].data.line.start.x, &objects[objectCount].data.line.start.y,
                                          &objects[objectCount].data.line.end1.x, &objects[objectCount].data.line.end1.y);
        } else if (objects[objectCount].type == 2) {
            fscanf(file, "%d", &objects[objectCount].data.polygon.numVertices);
            for (int j = 0; j < objects[objectCount].data.polygon.numVertices; j++) {
                fscanf(file, "%f %f", &objects[objectCount].data.polygon.vertices[j].x,
                                       &objects[objectCount].data.polygon.vertices[j].y);
            }
        }
        objectCount++;
    }

    fclose(file);
}

void keyboardCallback(unsigned char key, int x, int y) {
    if (key == 27) {

        exit(0);
    } else if (key == 'd' && selectedObject != -1) {

    }else if (key == 'p') {
        printSelectedObject();
    }else if (key == 'v') {
        printAllObjects();
    }else if(key == 's'){
        saveToFile("objects.txt");
    }else if(key == 'l'){
        loadFromFile("objects.txt");
    }else if(key == 'c'){
        deletarObjeto();
    }

}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawObjects();

    glFlush();
}

int main(int argc, char **argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);

    glutCreateWindow("OpenGL Application");


    glutDisplayFunc(display);
    glutMouseFunc(mouseCallback);
    glutKeyboardFunc(keyboardCallback);

    createMenu();

    glutMainLoop();
    return 0;
}
