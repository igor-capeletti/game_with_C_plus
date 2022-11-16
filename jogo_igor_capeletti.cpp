#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <time.h>

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;
//Compilar:
//g++ jogo_igor_capeletti.cpp -o executavel -lGL -lGLU -lglut -lm

//Executar:
//./executavel lista_objetos.txt normal
//ou 
//./executavel lista_objetos.txt
//ou
//./executavel

/*
Autor: Igor Ferrazza Capeletti
Disciplina: Computacao Grafica
Ano: 2021.2
Professor: Marcelo Thielo
*/

class Obj{
    private:
        int num_faces;
        float escala;
        float pos_x;
        float pos_y;
        float pos_z;
        float angulo_x;
        float angulo_y;
        float angulo_z;
        float red;
        float green;
        float blue;
        string nome_obj;
        
        vector <float> vx;
        vector <float> vy;
        vector <float> vz;
        vector <int> f_v1;
        vector <int> f_v2;
        vector <int> f_v3;
        vector <float> normal1;
        vector <float> normal2;
        vector <float> normal3;

    public:
        Obj();
        ~Obj();
        void carrega_objeto_3d(string opcao_obj, string nome_arquivo);
        void printa_objeto_3d();
        void desenha_objeto_tela(int opcao);
        void seta_escala_objeto(float nova_escala);
        void seta_cor_objeto(int r, int g, int b);
        void seta_posicao_objeto(int x, int y, int z);
        void rotaciona_objeto_3d(float thetax, float thetay, float thetaz);
        string retorna_nome_objeto();
        void movimenta_bola(float x, float y, float z);
};
//metodo construtor do objeto
Obj::Obj(){

}

//metodo destrutor do objeto
Obj::~Obj(){
    vx.clear();
    vy.clear();
    vz.clear();
    f_v1.clear();
    f_v2.clear();
    f_v3.clear();
    normal1.clear();
    normal2.clear();
    normal3.clear();
}


//------------ variaveis globais ------------
vector <Obj*> lista_objetos;
vector <Obj*> lista_obstaculos;
vector <vector<float>> lista_pos_obstaculos;

float x_atual, y_atual, z_atual;
float posicao_fase;
float tamanho_fase= 40;
float velocidade;
float obstaculo1[3];
float obstaculo2[3];
float obstaculo3[3];
float largura_cao= 150;


float angulo_x_bola, angulo_y_bola, angulo_z_bola, pos_x_bola, pos_y_bola, pos_z_bola;  //variaveis bola

//variaveis para utilizar na camera e tambem para rotacao e translacao dos objetos no cenario
float x_pos_atual, y_pos_atual, z_pos_atual, x_ap_atual, y_ap_atual, z_ap_atual;
float x_pos_cam, y_pos_cam, z_pos_cam;
float x_ap_cam, y_ap_cam, z_ap_cam;

//variaveis da iluminacao ambiente
float red_amb, green_amb, blue_amb;
float x_luz_amb, y_luz_amb, z_luz_amb;  //posicao da luz ambiente
GLfloat light0_ambient[] = {red_amb, green_amb, blue_amb, 0.2};    //intensidade das cores da luz0 ambiente
GLfloat light0_position[] = {x_luz_amb, y_luz_amb, z_luz_amb, 1.0 };
GLfloat globalAmbient[] = {0.4, 0.4, 0.4, 0.4};

//variaveis da iluminacao difusa
float red_dif, green_dif, blue_dif;
float x_luz_dif, y_luz_dif, z_luz_dif;  //posicao da luz difusa
GLfloat light2_diffuse[] = {red_dif, green_dif, blue_dif, 1.0};
GLfloat light2_position[] = {x_luz_dif, y_luz_dif, z_luz_dif , 1.0 }; //{0.0, 0.0, -50.0, 11.0};

//variaveis da iluminacao especular
float red_esp, green_esp, blue_esp;
float x_luz_esp, y_luz_esp, z_luz_esp;  //posicao da luz especular
GLfloat light1_specular[] = {red_esp, green_esp, blue_esp, 1.0};
GLfloat light1_position[] = {x_luz_esp, y_luz_esp, z_luz_esp, 1.0 };

//------------------------------------------------


void Obj::seta_escala_objeto(float nova_escala){
    escala= nova_escala;
}

void Obj::seta_posicao_objeto(int x, int y, int z){
    pos_x= x;
    pos_y= y;
    pos_z= z;
}

void Obj::seta_cor_objeto(int r, int g, int b){
    red= r;
    green= g;
    blue= b;
}


//metodo para girar objeto 3D em torno dos eixos X, Y e Z de acordo com valores recebidos (em radianos)
void Obj::rotaciona_objeto_3d(float thetax, float thetay, float thetaz){
    int i=0;
    float xt, yt, zt;
    if(thetay > 0.0){
        for(i=0; i < vx.size(); i++){   //rotaciona no eixo y
            xt= vx[i]*cos(thetay)-vz[i]*sin(thetay);
            zt= vx[i]*sin(thetay)+vz[i]*cos(thetay);
            vx[i]= xt;
            vz[i]= zt;
        }
    }
    if(thetax > 0.0){
        for(i=0; i < vx.size(); i++){   //rotaciona no eixo x
            yt= vy[i]*cos(thetax)-vz[i]*sin(thetax);
            zt= vy[i]*sin(thetax)+vz[i]*cos(thetax);
            vy[i]= yt;
            vz[i]= zt;
        }
    }
    if(thetaz > 0.0){
        for(i=0; i < vx.size(); i++){   //rotaciona no eixo z
            xt= vx[i]*cos(thetaz)-vy[i]*sin(thetaz);
            yt= vx[i]*sin(thetaz)+vy[i]*cos(thetaz);
            vx[i]= xt;
            vy[i]= yt;
        }
    }
    angulo_x= thetax;
    angulo_y= thetay;
    angulo_z= thetaz;
}

//metodo para ler uma imagem 3D (.obj)
void Obj::carrega_objeto_3d(string opcao_obj, string nome_arquivo){
    int i;
    num_faces=0;
    ifstream arq_input(nome_arquivo.c_str());
    string line;
    stringstream ss;

    if(arq_input.good()){
        nome_obj= opcao_obj;
        while(getline(arq_input, line,'\n').good()){
            ss.str("");
            ss.clear();
            ss.str(line);
            string subs;
            if(line[0] == 'v'){   // o primeiro caractere da linha indica um vertice
                getline(ss,subs,' ');// pula o caractere
                getline(ss,subs,' ');
                vx.push_back(stof(subs));
                getline(ss,subs,' ');
                vy.push_back(stof(subs));
                getline(ss,subs,' ');
                vz.push_back(stof(subs));
            }
            if(line[0] == 'f'){ // o primeiro caractere da linha indica uma face
                num_faces++;
                getline(ss,subs,' ');  // pula o caractere
                getline(ss,subs,' ');
                f_v1.push_back(stoi(subs)-1);   
                getline(ss,subs,' ');
                f_v2.push_back(stoi(subs)-1);
                getline(ss,subs,' ');
                f_v3.push_back(stoi(subs)-1);
            }
        }
        arq_input.close();
        for (i=0; i<num_faces; i++){
            float Px=vx[f_v2[i]]-vx[f_v1[i]];
            float Py=vy[f_v2[i]]-vy[f_v1[i]];
            float Pz=vz[f_v2[i]]-vz[f_v1[i]];

            float Qx=vx[f_v3[i]]-vx[f_v2[i]];
            float Qy=vy[f_v3[i]]-vy[f_v2[i]];
            float Qz=vz[f_v3[i]]-vz[f_v2[i]];

            float Ci = Py*Qz - Pz*Qy;
            float Cj = Px*Qz - Pz*Qx;
            float Ck = Px*Qy - Py*Qx;

            float modulo = sqrt(Ci*Ci+Cj*Cj+Ck*Ck);
            normal1.push_back(Ci/modulo);
            normal2.push_back(Cj/modulo);
            normal3.push_back(Ck/modulo);
        }
    }else{
        cout << "Erro de leitura!" << endl;
    }
}

string Obj::retorna_nome_objeto(){
    return nome_obj;
}

//metodo para printar na tela todos os dados lidos de uma imagem 3D (.obj)
void Obj::printa_objeto_3d(){
    int i=0;
    for(i=0; i < vx.size(); i++){
        cout << i << "\tv " << vx[i] << ' ' << vy[i] << ' ' << vz[i] << "\n";
    }
    for(i=0; i < f_v1.size(); i++){
        cout << i << "\tf " << f_v1[i]+1 << ' ' << f_v2[i]+1 << ' ' << f_v3[i]+1 << "\n";
    }
}

void carrega_todos_objetos(int argc, string arq_objetos){
    string resp = "s";
    string nome_objeto;
    string nome_objeto2;
    string comando;
    float nova_escala, x, y, z, r, g, b, rx, ry, rz;
    x_pos_atual=0.0, y_pos_atual= 0.0, z_pos_atual= 0.0;

    red_amb= 0.1, green_amb= 0.1, blue_amb= 0.1, x_luz_amb= 1000.0, y_luz_amb= 1000.0, z_luz_amb= -100.0; 
    red_dif= 1.0, green_dif= 1.0, blue_dif= 1.0, x_luz_dif= 1000.0, y_luz_dif= 100.0, z_luz_dif= -10000.0;  
    red_esp= 1.0, green_esp= 1.0, blue_esp= 1.0, x_luz_esp= 1000.0, y_luz_esp= 100.0, z_luz_esp= -1000.0;  

    //-------------------- usuario escolhe propriedades dos objetos por um arquivo: --------------------
    ifstream arq_input(arq_objetos);
    string line;
    stringstream ss;
    string subs;
    if(arq_input.good()){
        cout << "\n\nCarregando Objetos do arquivo " << arq_objetos << "...\n";
        while(getline(arq_input, line,'\n').good()){
            ss.str("");
            ss.clear();
            ss.str(line);
            if(!(line[0] == ' '|| line[0] == '#' || line[0] == '/')){
                Obj *novo_objeto = new Obj;
                
                getline(ss,subs,' ');
                comando = ("ctmconv "+subs+" buffer.obj --no-normals --no-texcoords");
                system(comando.c_str());
                comando = ("python3 limpa_lixo_arquivo_obj.py buffer.obj sufixo.obj");
                system(comando.c_str());
                comando = ("cp sufixo.obj "+subs);
                system(comando.c_str());
                novo_objeto->carrega_objeto_3d(subs, subs);
                nome_objeto= subs;
                getline(ss,subs,' ');
                nova_escala= stof(subs);
                getline(ss,subs,' ');
                x= stof(subs);
                getline(ss,subs,' ');
                y= stof(subs);
                getline(ss,subs,' ');
                z= stof(subs);
                novo_objeto->seta_escala_objeto(nova_escala);
                novo_objeto->seta_posicao_objeto(x, y, z);
                getline(ss,subs,' ');
                r= stof(subs);
                getline(ss,subs,' ');
                g= stof(subs);
                getline(ss,subs,' ');
                b= stof(subs);
                novo_objeto->seta_cor_objeto(r, g, b);
                getline(ss,subs,' ');
                rx= stof(subs);
                getline(ss,subs,' ');
                ry= stof(subs);
                getline(ss,subs,' ');
                rz= stof(subs);
                novo_objeto->rotaciona_objeto_3d(rx, ry, rz);
                lista_objetos.push_back(novo_objeto);
                if(nome_objeto == "objetos/espada.obj"){
                    cout << "igual ";
                    srand(time(NULL));
                    vector <float> posica_obst;
                    posica_obst.push_back(x);
                    posica_obst.push_back(y);
                    posica_obst.push_back(z);
                    lista_pos_obstaculos.push_back(posica_obst);

                    for(int i=3; i < tamanho_fase*3; i++){
                        Obj *obstaculo = new Obj;
                        vector <float> posica_obstaculo;
                        obstaculo->carrega_objeto_3d(nome_objeto, nome_objeto);
                        obstaculo->seta_escala_objeto(nova_escala);
                        x= -140+rand()%280;
                        z= i*500;
                        obstaculo->seta_posicao_objeto(x, y, z);
                        obstaculo->seta_cor_objeto(r, g, b);
                        obstaculo->rotaciona_objeto_3d(rx, ry, rz);
                        lista_objetos.push_back(obstaculo);
                        posica_obstaculo.push_back(x);
                        posica_obstaculo.push_back(y);
                        posica_obstaculo.push_back(z);
                        lista_pos_obstaculos.push_back(posica_obstaculo);
                    }
                }
            }
        }
        arq_input.close();
    }
}


//-------------------------- FUNCOES DA GLUT --------------------------
void desenha_grama(float tam, float x, float y, float z){
    glColor4f (0.0, 0.6, 0.0,1.0); 
    GLfloat cor[]={0.0, 0.6, 0.0, 0.0};     //cor da face atual
    glMaterialfv(GL_FRONT, GL_AMBIENT, cor);    //seta cor na face atual
    glEnable(GL_COLOR_MATERIAL);
    
    glPushMatrix();
    glBegin (GL_POLYGON);
        glVertex3f(-tam/2+x, y, -tam+z);
        glVertex3f(-tam/2+x, y, tam+z);
        glVertex3f(tam/2+x, y, tam+z);
        glVertex3f(tam/2+x, y, -tam+z);
    glEnd();
    glPopMatrix();
}
void desenha_inicio_fim(float tam, float x, float y, float z){
    glColor4f (1.0, 1.0, 1.0,1.0); 
    GLfloat cor[]={1.0, 1.0, 1.0, 1.0};     //cor da face atual
    glMaterialfv(GL_FRONT, GL_AMBIENT, cor);    //seta cor na face atual
    glEnable(GL_COLOR_MATERIAL);
    
    glPushMatrix();
    glBegin (GL_POLYGON);
        glVertex3f(-tam/2+x, y, -tam+z);
        glVertex3f(-tam/2+x, y, tam+z);
        glVertex3f(tam/2+x, y, tam+z);
        glVertex3f(tam/2+x, y, -tam+z);
    glEnd();
    glPopMatrix();
}

void Obj::desenha_objeto_tela(int opcao){
    glEnable(GL_COLOR_MATERIAL);
    glColor4f (0.0, 0.0, 0.0, 1.0);
    GLfloat cor[]={1.0, 0.0, 0.0, 1.0};    //cor da face atual

    glPushMatrix();
    for(int i=0; i<num_faces; i++){  
            if(red > green){
            if(green > blue){   //red > green > blue
                cor[0]= (float)i/(float)num_faces;
                cor[1]= ((float)i/(float)green)<0.5?(2.0*(float)i/(float)green):2.0-2.0*((float)i/(float)green);
                cor[2]= (float)i/(float)num_faces;
            }else{
                if(red > blue){     //red > blue > green
                    cor[0]= (float)i/(float)num_faces;
                    cor[2]= ((float)i/(float)green)<0.5?(2.0*(float)i/(float)green):2.0-2.0*((float)i/(float)green);
                    cor[1]= (float)i/(float)num_faces;
                }else{      //blue > red > green
                    cor[2]= (float)i/(float)num_faces;
                    cor[0]= ((float)i/(float)green)<0.5?(2.0*(float)i/(float)green):2.0-2.0*((float)i/(float)green);
                    cor[1]= (float)i/(float)num_faces;
                }
            }
        }else{ 
            if(green > blue){ 
                if(red > blue){     //green > red > blue
                    cor[1]= (float)i/(float)num_faces;
                    cor[0]= ((float)i/(float)green)<0.5?(2.0*(float)i/(float)green):2.0-2.0*((float)i/(float)green);
                    cor[2]= (float)i/(float)num_faces;
                }else{      //green > blue > red
                    cor[1]= (float)i/(float)num_faces;
                    cor[2]= ((float)i/(float)green)<0.5?(2.0*(float)i/(float)green):2.0-2.0*((float)i/(float)green);
                    cor[0]= (float)i/(float)num_faces;
                }
            }else{      //blue > green > red
                cor[2]= (float)i/(float)num_faces;
                cor[1]= ((float)i/(float)green)<0.5?(2.0*(float)i/(float)green):2.0-2.0*((float)i/(float)green);
                cor[0]= (float)i/(float)num_faces;
            }
        }

        glNormal3f(normal1[i],normal2[i],normal3[i]);
        glMaterialfv(GL_FRONT, GL_AMBIENT, cor);    //seta cor ambiente na face atual
        glBegin(GL_TRIANGLES);
            glVertex3f(pos_x+escala*vx[f_v1[i]], pos_y+escala*vy[f_v1[i]], pos_z+escala*vz[f_v1[i]]);
            glVertex3f(pos_x+escala*vx[f_v2[i]], pos_y+escala*vy[f_v2[i]], pos_z+escala*vz[f_v2[i]]);
            glVertex3f(pos_x+escala*vx[f_v3[i]], pos_y+escala*vy[f_v3[i]], pos_z+escala*vz[f_v3[i]]);
        glEnd();
    }
    glPopMatrix();
}

void timer(int value){
    glutPostRedisplay();
    if(angulo_z_bola > 6.28){
        angulo_z_bola= 0.00; 
    }else if(angulo_z_bola < 0.00){
        angulo_z_bola= angulo_z_bola+6.28;
    }
    
    velocidade= int(z_atual/10000)+4;
    //cout << velocidade << " ";
    z_atual+= 5*velocidade;     //atualiza velocidade da bola no cenario
    glutTimerFunc(10, timer, 0);
}

void Obj::movimenta_bola(float x, float y, float z){
    float distancia, distancia_x, distancia_z;
    pos_z= z;
    pos_x= x;

    //verifica se bola colidiu com algum dos objetos
    for(int i=0; i < lista_pos_obstaculos.size(); i++){
        if(pos_x < lista_pos_obstaculos[i][0]){
            distancia_x= pos_x-lista_pos_obstaculos[i][0];
        }else{
            distancia_x= lista_pos_obstaculos[i][0]-pos_x;
        }
        if(pos_z < lista_pos_obstaculos[i][2]){
            distancia_z= pos_z-lista_pos_obstaculos[i][2];
        }else{
            distancia_z= lista_pos_obstaculos[i][2]-pos_z;
        }
        distancia = sqrt((distancia_x*distancia_x) + (distancia_z*distancia_z));
        
        if(abs(distancia) < 20){   //se distancia entre a bola e algum dos objetos for menor, colidiu! 
            cout << "\n\nBateu!!!";
            pos_z= 0;
            pos_x= 0;
            z_atual= 0;
        }
    }
    desenha_objeto_tela(1);
    rotaciona_objeto_3d(angulo_x_bola+0.08+(velocidade*0.02), angulo_y_bola, angulo_z_bola);
    angulo_z_bola= 0.0;
}

void display(void){
    int i;
    float e;
    GLfloat rot_x=0.0, rot_y=0.0;
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //---------------- ILUMINACOES ----------------
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);       //atribui a luz LIGHT0, com o atributo ambiente, com as cores light0_ambient
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);     //atribui posicao da luz ambiente
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    glColorMaterial(GL_FRONT, GL_AMBIENT);                  //ativa luz ambiente nos objetos, nas faces frontais
    glEnable(GL_LIGHT0);
                                    
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);     //atribui a luz LIGHT1, com o atributo especular, com as cores light1_specular
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);     //atribui posicao da luz especular
    glColorMaterial(GL_FRONT, GL_SPECULAR);                 //ativa luz especular nos objetos, nas faces frontais
    glEnable(GL_LIGHT1);
                                        
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);       //atribui a luz LIGHT2, com o atributo diffuse, com as cores light2_diffuse
    glLightfv(GL_LIGHT2, GL_POSITION, light2_position);     //atribui posicao da luz difusa
    glColorMaterial(GL_FRONT, GL_DIFFUSE);                  //ativa luz difusa nos objetos, nas faces frontais
    glEnable(GL_LIGHT2);                                    //ativa luz difusa

    glEnable(GL_COLOR_MATERIAL);    //ativa cor dos objetos
    glEnable(GL_LIGHTING);          //liga todas as iluminacoes

    //---------------- CAMERA ----------------
    gluLookAt(10+x_atual, 100, -300+z_atual, 10+x_atual, 0, 0+z_atual, 0, 1.0, 0.0);
    glScalef(1.0, 1.0, 1.0);

    
    //---------------- CENARIO ----------------
    lista_objetos[0]->movimenta_bola(x_atual,y_atual,z_atual);      //desenha bola na tela

    for(i=1; i<lista_objetos.size(); i++){          //desenha obstaculos na cenario
        lista_objetos[i]->desenha_objeto_tela(1);
    }
    desenha_inicio_fim(300, 0, 0, 0);
    desenha_inicio_fim(300, 0, 0, 600);
    desenha_inicio_fim(300, 0, 0, (((tamanho_fase*2)+tamanho_fase/2))*600);
    for(i=2; i<(((tamanho_fase*2)+tamanho_fase/2)); i++){     //desenha todo o chao do cenario
        desenha_grama(300, 0, 0, i*600);
    }

    glFlush();
    glutSwapBuffers();
    glDisable(GL_TEXTURE_2D);

    //fecha jogo ao chegar no final
    if(z_atual > (tamanho_fase*3*500)-400){
        cout << "\n\n\nParabens!!! Voce ganhou o jogo!!!\n\n";
        
        //limpar vectores dentro de cada objeto e limpa o vetor de objetos
        for(i=0; i < lista_objetos.size(); i++){
            lista_objetos[i]->~Obj();
        }
        lista_objetos.clear();
        lista_pos_obstaculos.clear();
        exit(0);
    }
}


void myReshape(int w, int h){
    float aspectRatio = (float)w/(float)h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-50,50,-50,50,-50,50);
    gluPerspective(45,aspectRatio,-20,20);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void myInit(void){
    glClearColor (0.0, 0.0, 0.4, 0.4);
    //glShadeModel (GL_FLAT);
    glShadeModel(GL_SMOOTH);
    glFrontFace(GL_CW);    
    glCullFace(GL_FRONT);    //Estas tres fazem o culling funcionar
    glEnable(GL_CULL_FACE); 

    glClearDepth(100.0f);
    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LEQUAL);
    glDepthRange(200,-200);   
}

//comandos do teclado para andar com a camera
void processNormalKeys(unsigned char key, int x, int y){
    if(key == 27){ //tecla Escape - fechar a tela de visualizacao
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        exit(0);
    }
    glutPostRedisplay();
}

//comandos especiais do teclado para andar com a camera
void processSpecialKeys(int key, int x, int y){
    if(key == GLUT_KEY_RIGHT){  //tecla seta para direita, vai para direita no cenario
        if(x_atual-5 >= -115){
            x_atual-=5;
        }
        angulo_z_bola+=0.08; 
    }
    if(key == GLUT_KEY_LEFT){   //tecla com seta para esquerda, vai para esquerda no cenario
        if(x_atual+5 <= 126){
            x_atual+=5;
        }
        angulo_z_bola-=0.08; 
    }
}



int main(int argc, char **argv){
    int i= argc;
    char **argumentos= argv;
    velocidade= 1;

    carrega_todos_objetos(i, "objetos/lista_objetos.txt");
    

    //--------------------- chamadas das funcoes da GLUT ---------------------
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (1000, 1000);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Trabalho 3 - Igor Capeletti - Computacao Grafica");
    glEnable (GL_BLEND | GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    myInit();
    glutDisplayFunc(display);
    glutReshapeFunc(myReshape);
    glutTimerFunc(10, timer, 0);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);
    //glutMouseFunc(Mouse);

    glutMainLoop();

    //limpar vectores dentro de cada objeto e limpa o vetor de objetos
    for(i=0; i < lista_objetos.size(); i++){
        lista_objetos[i]->~Obj();
    }
    lista_objetos.clear();
    lista_pos_obstaculos.clear();
    return 0;
}
