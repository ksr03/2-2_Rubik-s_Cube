#include <stdlib.h>
#include <GL/glut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define num_of_cube 8   //ルービックキューブを構成する部品の個数

int width, height;      //ウィンドウの幅・高さ

int in_rotation = 0;   //ルービックキューブが回転していなければ0、
                        //全体の回転で1、上半分の回転で2、下半分の回転で3、右半分の回転で4、左半分の回転で5とする
bool in_rotation_up = false;   //ルービックキューブが上方向に回転途中であればtrue、それ以外はfalse
bool in_rotation_down = false;   //ルービックキューブが下方向に回転途中であればtrue、それ以外はfalse
bool in_rotation_right = false;   //ルービックキューブが右方向に回転途中であればtrue、それ以外はfalse
bool in_rotation_left = false;   //ルービックキューブが左方向に回転途中であればtrue、それ以外はfalse

//関数のプロトタイプ宣言
void reset_Cubes(void);
void draw_part(int cube_number), draw_Cubes(double r), draw_guide(void);
void translate_matrix(int cube_number), rotation_matrix(int cube_number);
void change_position(void), change_direction(void), change_cube_group(void);
bool include_cube_group(int cube_number);


/*座標リスト*/
GLdouble position[8][3] = {
    {-1., 1., 1},   // 0   //上半分
    {1., 1., 1},    // 1
    {-1., 1., -1},  // 2
    {1., 1., -1},   // 3
    {-1., -1., 1},  // 4    //下半分
    {1., -1., 1},   // 5
    {-1., -1., -1.},// 6
    {1., -1., -1}   // 7
};

int get_position_num(GLdouble* p) {             //pの座標と一致する座標リストのインデックスを返す
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 3; j++) {
            if (p[j] != position[i][j])
                break;
            else if (j == 2)
                return i;
        }
    }
    return -1;
}

/*縦回転・横回転の向きのリスト*/
//1はx軸、2はy軸、3はz軸方向とする、負の方向はマイナスを付ける
int rotation_vertical[] = { 2, -3, -2, 3 };     //縦回転
int rotation_horizontal[] = { 1, 3, -1, -3 };   //横回転

int get_rotation_num(int d) {      //変数 d のrotation_vertical又はrotation_horizontal内でのインデックスを返す
    static int i;
    if (in_rotation_up || in_rotation_down) {   //縦回転の場合
        for (i = 0; i < 4; i++) {
            if (d == rotation_vertical[i])
                return i;
        }
    }
    if (in_rotation_right || in_rotation_left) {
        for (i = 0; i < 4; i++) {
            if (d == rotation_horizontal[i])
                return i;
        }
    }
    return -1;
}

/*色リスト*/
GLfloat color[7][3] = {
    {0.9, 0.9, 0.9},        // 0. 白
    {1., 0., 0.2},          // 1. 赤
    {0.35, 0.95, 0.1},      // 2. 緑
    {0.1, 0.4, 1.},         // 3. 青
    {0., 0.8, 0.85},        // 4. 水色
    {0.9, 0.5, 0.1},        // 5. オレンジ
    {0.9, 0.9, 0.15},       // 6. 黄色
};

/*ライトに関する設定*/
GLfloat light0pos[] = { 0.0, 3.0, 10.0, 1.0 };  //ライトの位置
GLfloat light_color[] = { 1.0, 0.8, 0.9, 1.0 }; //ライトの色
GLfloat lightdire[] = { 0.0, -1.0, -1.0 };      //ライトの向き

/*ルービックキューブの部品の設定*/
int cube_position[8];  //各部品の位置

int cube_direction[8]; //各部品の向き

int cube_color[8][3]{  //各部品の色番号
    {4, 1, 5},  // 0
    {4, 6, 1},  // 1
    {4, 5, 2},  // 2
    {4, 2, 6},  // 3
    {3, 5, 1},  // 4
    {3, 1, 6},  // 5
    {3, 2, 5},  // 6
    {3, 6, 2}   // 7
};

int cube_group[4][4];   //ルービックキューブを上下・左右に分割した際に、どの部品がどのグループに属しているか
                        //cube_group[0]は上半分のグループに属している部品の番号
                        //cube_group[1]は下半分のグループに属している部品の番号
                        //cube_group[2]は右半分のグループに属している部品の番号
                        //cube_group[3]は左半分のグループに属している部品の番号

void reset_Cubes(void) {    //各部品の位置と向きを初期化する
    static int default_position[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    static int default_direction[8] = { 2, 2, 2, 2, -2, -2, -2, -2 };
    if (in_rotation == 0) {
        for (int i = 0; i < num_of_cube; i++) {
            cube_position[i] = default_position[i];
            cube_direction[i] = default_direction[i];
        }
    }
}

/*各部品の描画*/
void draw_part(int cube_number) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color[0]);
    glutSolidCube(1.9);
    glPushMatrix();
        glTranslated(0., 0.1, 0.);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color[cube_color[cube_number][0]]);
        glutSolidCube(1.8);
    glPopMatrix();
    glPushMatrix();
        glTranslated(0.1, 0., 0.);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color[cube_color[cube_number][1]]);
        glutSolidCube(1.8);
    glPopMatrix();
    glPushMatrix();
        glTranslated(0., 0., 0.1);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color[cube_color[cube_number][2]]);
        glutSolidCube(1.8);
    glPopMatrix();
}

/*ルービックキューブの描画*/
void draw_Cubes(double r) {
    static int i;
    static bool include;    //各部品が回転途中ならtrue、そうでなければfalse

    for (i = 0; i < num_of_cube; i++) {
        include = include_cube_group(i);
        glPushMatrix();
            switch (in_rotation) {
            case 0:     //回転途中でない場合
                break;
            case 1:     //回転している場合
            case 2:
            case 3:
            case 4:
            case 5:
                if (in_rotation_up && include)            //上方向の回転
                    glRotated(-r, 1., 0., 0.);
                else if (in_rotation_down && include)     //下方向の回転
                    glRotated(r, 1., 0., 0.);
                else if (in_rotation_right && include)    //右方向の回転
                    glRotated(r, 0., 1., 0.);
                else if (in_rotation_left && include)     //左方向の回転
                    glRotated(-r, 0., 1., 0.);
                break;
            default:
                break;
            }
        
            translate_matrix(i);  //部品の移動
            rotation_matrix(i);   //部品の回転
            draw_part(i);         //部品の描画
        glPopMatrix();
    }
}

bool include_cube_group(int cube_number) {  //cube_numberの部品が回転しているグループに含まれていればtrue、含まれなければfalseを返す
    static int i;
    if (in_rotation == 0)
        return false;
    else if (in_rotation == 1)   //ルービックキューブ全体が回転している場合
        return true;
    else if ((2 <= in_rotation) && (in_rotation <= 5)) {    //ルービックキューブの半分が回転している場合
        for (i = 0; i < 4; i++) {
            if (cube_number == cube_group[in_rotation - 2][i]) {
                return true;
            }
        }
    }
    return false;
}

/*移動用の変換行列の作成*/
void translate_matrix(int cube_number){
    glTranslated(position[cube_position[cube_number]][0], position[cube_position[cube_number]][1], position[cube_position[cube_number]][2]);
}

/*回転用の変換行列の作成*/
void rotation_matrix(int cube_number) {
    int rx_times = 0; //x軸中心に回転させる回数
    int ry_times = 0; //y軸中心に回転させる回数
    int r_axis = 0;     //任意の軸を表す変数、1の場合をx軸、3の場合をz軸とする
    int r_times = 0;     //任意の軸を中心に回転させる回数

    switch (cube_direction[cube_number]) {
    case 2:     //draw_partで生成される部品の向きがy軸方向を向いている場合
        switch (cube_position[cube_number]) {
        case 0:     ry_times = 3; break;
        case 1:     ry_times = 0; break;
        case 2:     ry_times = 2; break;
        case 3:     ry_times = 1; break;
        }
        break;
    case -2:    //draw_partで生成される部品の向きがy軸の負の方向を向いている場合
        rx_times = 2;
        switch (cube_position[cube_number]) {
        case 4:     ry_times = 2; break;
        case 5:     ry_times = 3; break;
        case 6:     ry_times = 1; break;
        case 7:     ry_times = 0; break;
        }
        break;
    case 1:     //draw_partで生成される部品の向きがx軸方向を向いている場合
        rx_times = 1;
        ry_times = 1;
        r_axis = 1;
        switch (cube_position[cube_number]) {
        case 1:     r_times = 2; break;
        case 3:     r_times = 1; break;
        case 5:     r_times = 3; break;
        case 7:     r_times = 0; break;
        }
        break;
    case 3:     //draw_partで生成される部品の向きがz軸方向を向いている場合
        rx_times = 1;
        ry_times = 0;
        r_axis = 3;
        switch (cube_position[cube_number]) {
        case 0:     r_times = 2; break;
        case 1:     r_times = 1; break;
        case 4:     r_times = 3; break;
        case 5:     r_times = 0; break;
        }
        break;
    case -1:    //draw_partで生成される部品の向きがx軸の負の方向を向いている場合
        rx_times = 1;
        ry_times = 3;
        r_axis = 1;
        switch (cube_position[cube_number]) {
        case 0:     r_times = 3; break;
        case 2:     r_times = 2; break;
        case 4:     r_times = 0; break;
        case 6:     r_times = 1; break;
        }
        break;
    case -3:    //draw_partで生成される部品の向きがz軸の負の方向を向いている場合
        rx_times = 1;
        ry_times = 2;
        r_axis = 3;
        switch (cube_position[cube_number]) {
        case 2:     r_times = 3; break;
        case 3:     r_times = 2; break;
        case 6:     r_times = 0; break;
        case 7:     r_times = 1; break;
        }
        break;
    default:
        break;
    }
    
    if (r_axis != 0) {  //任意の軸中心の回転
        if (r_axis == 1) {
            glRotated((GLdouble)r_times * 90.0, 1., 0., 0.);
        }
        else if (r_axis == 3) {
            glRotated((GLdouble)r_times * 90.0, 0., 0., 1.);
        }
    }
    if (ry_times > 0) { //y軸中心の回転
        glRotated(ry_times * 90.0, 0., 1., 0.);
    }
    if (rx_times > 0) { //x軸中心の回転
        glRotated((GLdouble)rx_times * 90.0, 1., 0., 0.);
    }
}

/*ルービックキューブの情報の更新*/
void change_position(void) {    //cube_positionの更新を行う
    static int i, j;
    static GLdouble x, y, z;     //一時的に座標を保存しておく変数
    static double degree;      //回転する角度
    static GLdouble p[3];        //更新前の座標

    if (in_rotation_up || in_rotation_down) {       //縦方向の回転の場合
        if (in_rotation_up) degree = M_PI / 2;      //z-y平面上で90°回転する
        if (in_rotation_down) degree = -M_PI / 2;   //z-y平面上で-90°回転する

        for (i = 0; i < num_of_cube; i++) {
            if (include_cube_group(i)) {
                for (j = 0; j < 3; j++)
                    p[j] = position[cube_position[i]][j];   //更新前の座標を取得

                y = p[1];
                z = p[2];
                p[1] = z * sin(degree);                 //新しいy座標の計算
                p[2] = -1 * y * sin(degree);            //新しいz座標の計算
                cube_position[i] = get_position_num(p); //cube_positionの更新
            }
        }
    }

    if (in_rotation_right || in_rotation_left) {    //横方向の回転の場合
        if (in_rotation_right) degree = -M_PI / 2;  //x-z平面上で-90°回転する
        if (in_rotation_left) degree = M_PI / 2;    //x-z平面上で-90°回転する

        for (i = 0; i < num_of_cube; i++) {
            if (include_cube_group(i)) {
                for (j = 0; j < 3; j++)
                    p[j] = position[cube_position[i]][j];   //更新前の座標を取得

                x = p[0];
                z = p[2];
                p[0] = -1 * z * sin(degree);            //新しいx座標の計算
                p[2] = x * sin(degree);                 //新しいz座標の計算
                cube_position[i] = get_position_num(p); //cube_positionの更新
            }
        }
    }      
}

void change_direction(void) {  //cubes_directionの更新を行う
    static int i, j;
    static int r;   //反時計回りで1、時計回りで-1

    if (in_rotation_up || in_rotation_down) {   //縦方向（z-y平面）の回転の場合
        if (in_rotation_up)     r = 1;
        if (in_rotation_down)   r = -1;

        for (i = 0; i < num_of_cube; i++) {
            if (include_cube_group(i)) {
                j = get_rotation_num(cube_direction[i]);    //rotation_verticalのインデックスを取得
                if (j != -1) {
                    j += r;                                 //回転後の向きのrotation_verticalのインデックスを取得
                    if (j < 0) j = 3;
                    j %= 4;
                    cube_direction[i] = rotation_vertical[j];   //cube_directionの更新
                }
            }
        }
    }

    if (in_rotation_right || in_rotation_left) {    //横方向（x-z平面）の回転の場合
        if (in_rotation_right)  r = -1;
        if (in_rotation_left)   r = 1;

        for (i = 0; i < num_of_cube; i++) {
            if (include_cube_group(i)) {
                j = get_rotation_num(cube_direction[i]);    //rotation_verticalのインデックスを取得
                if (j != -1) {
                    j += r;                                 //回転後の向きのrotation_horizontalのインデックスを取得
                    if (j < 0) j = 3;
                    j %= 4;
                    cube_direction[i] = rotation_horizontal[j];   //cube_directionの更新
                }
            }
        }
    }
}

void change_cube_group(void) {  //cube_groupの更新を行う
    int cube_number;
    int i = 0, j = 0, k = 0, l = 0;

    for (cube_number = 0; cube_number < num_of_cube; cube_number++) {
            if (position[cube_position[cube_number]][1] == 1.)      //cube_numberのy座標が正の場合、上半分のグループに格納する
                cube_group[0][i++] = cube_number;   
            else                                                    //それ以外の場合、下半分のグループに格納する
                cube_group[1][j++] = cube_number;    
            if (position[cube_position[cube_number]][0] == 1.)      //cube_numberのx座標が正の場合、右半分のグループに格納する
                cube_group[2][k++] = cube_number;   
            else                                                    //それ以外の場合、左半分のグループに格納する
                cube_group[3][l++] = cube_number;   
    }
}

void draw_guide(void) { //画面上のガイドの描画
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color[0]);
    glPushMatrix();
        glScaled(0.7, 0.05, 0.);
        glutSolidCube(1.);
    glPopMatrix();
    glPushMatrix();
        glScaled(0.05, 0.7, 0.);
        glutSolidCube(1.);
    glPopMatrix();
}

void idle(void)
{
    glutPostRedisplay();
}

void display(void)
{
    static double r = 0.0; //アニメーションでの回転角

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    draw_guide();   //画面中央にガイドを表示
    gluLookAt(2.0, 2.0, 8.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //視点の設定

    if ((in_rotation != 0) && (r <= 90)) {    //回転している途中の処理
        r += 0.2;
    }
    else if (r > 90) { //回転が終わった時の処理
        change_position();     //cube_positionの更新
        change_direction();     //cube_directionの更新
        change_cube_group();    //cube_groupの更新

        in_rotation_up = false;
        in_rotation_down = false;
        in_rotation_right = false;
        in_rotation_left = false;

        r = 0.0;
        in_rotation = 0;
    }
    draw_Cubes(r);


    glutSwapBuffers();
}

void resize(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-5, 5, -5, 5, -30.0, 30.0);

    glMatrixMode(GL_MODELVIEW);

    width = w;  //ウィンドウの幅を取得
    height = h; //ウィンドウの高さを取得
}

void mouse(int button, int state, int x, int y)
{
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if ((state == GLUT_DOWN) && (in_rotation == 0)) {
            if ((y < height / 2) && (x < width / 2)) {   //画面左上にカーソルがあるとき
                in_rotation = 5;
                in_rotation_up = true;
            }
            else if ((y < height / 2) && (x > width / 2)) {  //画面右上にカーソルがあるとき
                in_rotation = 4;
                in_rotation_up = true;
            }
            else if ((y > height / 2) && (x < width / 2)) {  //画面左下にカーソルがあるとき
                in_rotation = 5;
                in_rotation_down = true;
            }
            else if ((y > height / 2) && (x > width / 2)) {  //画面右下にカーソルがあるとき
                in_rotation = 4;
                in_rotation_down = true;
            }
        }
        break;
    case GLUT_RIGHT_BUTTON:
        if ((state == GLUT_DOWN) && (in_rotation == 0)) {
            if ((y < height / 2) && (x < width / 2)) {   //画面左上にカーソルがあるとき
                in_rotation = 2;
                in_rotation_left = true;
            }
            else if ((y < height / 2) && (x > width / 2)) {  //画面右上にカーソルがあるとき
                in_rotation = 2;
                in_rotation_right = true;
            }
            else if ((y > height / 2) && (x < width / 2)) {  //画面左下にカーソルがあるとき
                in_rotation = 3;
                in_rotation_left = true;
            }
            else if ((y > height / 2) && (x > width / 2)) {  //画面右下にカーソルがあるとき
                in_rotation = 3;
                in_rotation_right = true;
            }
        }
        break;
    case GLUT_MIDDLE_BUTTON:
        if (state == GLUT_DOWN) {

        }
        break;
    default:
        break;
    }
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'r':   //rキーが押されたらルービックキューブをリセット
        if (in_rotation == 0) {
            reset_Cubes();                  //cube_positionとcube_directionの初期化
            change_cube_group();            //配列cube_groupの初期化
        }
        break;
    case 'q':   //qキーが押されたらプログラムを終了
    case 'Q':
    case '\033':
        exit(0);
    default:
        break;
    }
}

void arrow_keys(int key, int x, int y)  //矢印キー用の関数
{
    switch (key) {
    case GLUT_KEY_UP:       //上矢印キーが押されたとき、上回転を行う
        if (in_rotation == 0) {
            in_rotation = 1;
            in_rotation_up = true;
        }
        break;
    case GLUT_KEY_DOWN:     //下矢印キーが押されたとき、下回転を行う
        if (in_rotation == 0) {
            in_rotation = 1;
            in_rotation_down = true;
        }
        break;
    case GLUT_KEY_RIGHT:    //右矢印キーが押されたとき、右回転を行う
        if (in_rotation == 0) {
            in_rotation = 1;
            in_rotation_right = true;
        }
        break;
    case GLUT_KEY_LEFT:     //左矢印キーが押されたとき、左回転を行う
        if (in_rotation == 0) {
            in_rotation = 1;
            in_rotation_left = true;
        }
        break;
    default:
        break;
    }
}

void init(void)
{
    glClearColor(0.95, 0.95, 1., 1.0);   //背景色を設定
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    /*ライトの設定*/
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light0pos);   //ライトの位置を設定
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);    //ライトを白色に設定
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 300.0);    //ライトの大きさを300に設定
    
    reset_Cubes();                  //cube_positionとcube_directionの初期化
    change_cube_group();            //配列cube_groupの初期化
}

void timer(int n) { //一定時間毎にglutIdleFuncを呼び出すことで、アニメーションを作成する
    glutIdleFunc(idle);
    glutTimerFunc(100, timer, 0);   //100ミリ秒後に関数timerを呼び出す
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(800, 800);   //ウィンドウのサイズを設定
    glutCreateWindow("2×2ルービックキューブ");   //ウィンドウの名前を設定
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(arrow_keys);    //矢印キー用のコールバック関数の設定
    init();
    glutTimerFunc(100, timer, 0);   //100ミリ秒後に関数timerを呼び出す
    glutMainLoop();
    return 0;
}