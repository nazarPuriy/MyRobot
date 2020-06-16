#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include <posicion.h>
#include <stdio.h>
#include <unistd.h>
#include <dyn/dyn_app_sensor.h>

#include "main.h"
#include "dyn/dyn_app_common.h"
#include "dyn/dyn_app_motors.h"
#include "dyn/dyn_app_sensor.h"
#include "dyn_test/dyn_emu.h"
#include "dyn_test/b_queue.h"
#include "joystick_emu/joystick.h"
#include "habitacion_001.h"

void findWall();
void move();
uint8_t estado = Ninguno, estado_anterior = Ninguno, finalizar = 0;
uint32_t indice;

/**
 * main.c
 */
int main(void) {
    pthread_t tid, jid;
    uint8_t tmp;

    //Init queue for TX/RX data
    init_queue(&q_tx);
    init_queue(&q_rx);

    //Start thread for dynamixel module emulation
    // Passing the room information to the dyn_emu thread
    pthread_create(&tid, NULL, dyn_emu, (void *) datos_habitacion);
    pthread_create(&jid, NULL, joystick_emu, (void *) &jid);

    printf("\nDimensiones habitacion %d ancho x %d largo mm2\n", ANCHO, LARGO);
    printf("En memoria: %I64u B = %I64u MiB\n", sizeof(datos_habitacion), sizeof(datos_habitacion) >> 20);

    printf("Pulsar 'q' para terminar, qualquier tecla para seguir\n");
    fflush(stdout);//	return 0;


    move();//Iniciamos el movimiento del robot.

    while (estado != Quit) {
        if (simulator_finished) {
            break;
        }
        Get_estado(&estado, &estado_anterior);
        if (estado != estado_anterior) {
            Set_estado_anterior(estado);
            printf("estado = %d\n", estado);
            switch (estado) {
                case Sw1:
                    printf("Boton Sw1 ('a') apretado\n");
                    dyn_led_control(1, 1); //Probaremos de encender el led del motor 2
                    printf("\n");
                    break;
                case Sw2:
                    printf("Boton Sw2 ('s') apretado\n");
                    dyn_led_control(1, 0); //Probaremos de apagar el led del motor 2
                    printf("\n");
                    break;
                case Up:

                    break;
                case Down:

                    break;
                case Left:
                    //Comprobaremos si detectamos las esquinas de la pared izquierda:
                    printf("Esquina inferior izquierda:\n");
                    printf("(1, 1): %d (fuera pared)\n", obstaculo(1, 1, datos_habitacion));
                    printf("(0, 1): %d (pared izq.)\n", obstaculo(0, 1, datos_habitacion));
                    printf("(1, 0): %d (pared del.)\n", obstaculo(1, 0, datos_habitacion));
                    printf("(0, 0): %d (esquina)\n", obstaculo(0, 0, datos_habitacion));
                    printf("Esquina superior izquierda:\n");
                    printf("(1, 4094): %d (fuera pared)\n", obstaculo(1, 4094, datos_habitacion));
                    printf("(0, 4094): %d (pared izq.)\n", obstaculo(0, 4094, datos_habitacion));
                    printf("(1, 4095): %d (pared fondo.)\n", obstaculo(1, 4095, datos_habitacion));
                    printf("(0, 4095): %d (esquina)\n", obstaculo(0, 4095, datos_habitacion));
                    break;
                case Right:
                    //Comprobaremos si detectamos las esquinas de la pared derecha:
                    printf("Esquina inferior derecha:\n");
                    printf("(4094, 1): %d (fuera pared)\n", obstaculo(4094, 1, datos_habitacion));
                    printf("(4094, 0): %d (pared del.)\n", obstaculo(4094, 0, datos_habitacion));
                    printf("(4095, 1): %d (pared der.)\n", obstaculo(4095, 1, datos_habitacion));
                    printf("(4095, 0): %d (esquina)\n", obstaculo(4095, 0, datos_habitacion));
                    printf("Esquina superior derecha:\n");
                    printf("(4094, 4094): %d (fuera pared)\n", obstaculo(4094, 4094, datos_habitacion));
                    printf("(4094, 4095): %d (pared fondo)\n", obstaculo(4094, 4095, datos_habitacion));
                    printf("(4095, 4094): %d (pared der.)\n", obstaculo(4095, 4094, datos_habitacion));
                    printf("(4095, 4095): %d (esquina)\n", obstaculo(4095, 4095, datos_habitacion));
                    break;
                case Center:

                    break;
                case Quit:
                    printf("Adios!\n");
                    break;
                    //etc, etc...
            }
            fflush(stdout);
        }
    }
    //Signal the emulation thread to stop
    pthread_kill(tid, SIGTERM);
    pthread_kill(jid, SIGTERM);
    printf("Programa terminado\n");
    fflush(stdout);
}

void acelerate(bool more,int quantity){
    printf("\nACELERATING\n");
    uint8_t center;
    //Sumem fins arribar velocitat quantity*10;
    bool balance;
    int i;
    for(i=0; i<quantity; i++){
        dyn_sensor_read_center(3,&center);
        //Mirem sempre que no anem a col·lisionar.
        if(center<10){
            printf("\n\nACELERATION NOT COMPLETED");
            dyn_move(DYN_ID_MOTORL,40,false);
            dyn_move(DYN_ID_MOTORR,40,false);
            return;
        }

        if(balance){
            if(more){
                addSpeed(DYN_ID_MOTORL);
                addSpeed(DYN_ID_MOTORR);
            }else{
                subSpeed(DYN_ID_MOTORL);
                subSpeed(DYN_ID_MOTORR);
            }
            balance = false;
        }else{
            if(more){
                addSpeed(DYN_ID_MOTORR);
                addSpeed(DYN_ID_MOTORL);
            }else{
                subSpeed(DYN_ID_MOTORL);
                subSpeed(DYN_ID_MOTORR);
            }
            balance = true;
        }
    }
}

//Función que sirve para amntenerse a una distancia prudente de la pared.
void adhere(uint8_t center,uint8_t right,uint8_t left,int8_t centerAnt,uint8_t rightAnt,uint8_t leftAnt,uint16_t motorl,uint16_t motorr){
    printf("\nADHERING");
    //Usaremos para determinar si estamos cerca o lejos de la izquierda
    int lejos = 8;
    int cerca = 6;
    int cerca2 = 14;

    //Positivo indica que nos hemos alejado esa distancia del obstucalo.
    int leftDistance;
    int rightDistance;
    int centerDistance;

    //Indican a que lado estamos girando.
    bool rotatingLeft;
    bool rotatingRight;
    bool rotatingStraight;

    //Si postitiu ens hem allunyat
    leftDistance=left-leftAnt;
    rightDistance=right-rightAnt;
    centerDistance=center-centerAnt;
    rotatingLeft = false;
    rotatingRight = false;
    rotatingStraight = false;
    if(motorl==motorr){
        rotatingStraight = true;
    }else if(motorr>motorl){
        rotatingLeft = true;
    }else{
        rotatingRight = true;
    }

    //Un cop tenim totes les variables que ens indiquen que estem fent anem a actuar.


    //Si estem massa aprop. Girem dreta o augmentem velocitat roda esquerra.
    if(left<cerca || center<cerca2){
        printf("\nAlgun massa aprop");
        //Si no ens estem allunyant o ens allunyem pero encara estem massa aprop
        if(!(leftDistance>0)||(leftDistance>0&&(left<4||center<4))){
            //Mirem de no girar massa.
            if(motorl-motorr<20){
                //Estamos girando izq, hay que ir recto. Ponemos rueda izq a derecha.
                if(rotatingLeft){
                    dyn_move(DYN_ID_MOTORL,motorr,false);
                    motorl=motorr;
                }
                if(motorl<102){
                    addSpeed(DYN_ID_MOTORL);
                }else{
                    subSpeed(DYN_ID_MOTORR);
                }
            }
        }else{
            if(motorr>motorl){
                dyn_move(DYN_ID_MOTORL,motorr,false);
            }else{
                dyn_move(DYN_ID_MOTORR,motorl,false);
            }
        }

    //Si estem massa lluny. Ens apropem, girant esquerra o més velocitat dreta.
    }else if(left>lejos && !(center<cerca2)){
        printf("\nMassa lluny");
        //Si ens estem allunyant.
        if(leftDistance>=0||((!leftDistance>=0)&&left>9)){
            //Mirem que no estiguem girant massa rápud.
            if(motorr-motorl<20){
                //Estem girant dreta. Hem de ficar roda dreta a velocitat de l'esquerra.
                if (rotatingRight) {
                    dyn_move(DYN_ID_MOTORR, motorl, false);
                    motorr = motorl;
                }
                if (motorr < 102) {
                    addSpeed(DYN_ID_MOTORR);
                } else {
                    subSpeed(DYN_ID_MOTORL);
                }
            }
        }else{
            if(motorr>motorl){
                dyn_move(DYN_ID_MOTORL,motorr,false);
            }else{
                dyn_move(DYN_ID_MOTORR,motorl,false);
            }
        }
    }else{
        if(motorr>motorl){
            dyn_move(DYN_ID_MOTORL,motorr,false);
        }else{
            dyn_move(DYN_ID_MOTORR,motorl,false);
        }
    }
    //Para no acercarnos demasiado cuando la pared gira a la derecha.
    if(center<4){
        addSpeed(DYN_ID_MOTORL);
    }
}

//Función que orienta al robot en el sentido de la pared más cercana
void findBestDirection(){
    printf("\nFINDING THE CLOSEST WALL");
    int min = 300;
    int i = 0;
    uint8_t center;
    //Hacemos que el robot de una vuelta entera.
    dyn_move(DYN_ID_MOTORL,15,false);
    dyn_move(DYN_ID_MOTORR,0,false);
    while(i<500){
        i++;
        dyn_sensor_read_center(3, &center);
        if(center<min){
            min = center;
        }
    }

    //Una vez leídos todos vamos a ponernos en dirección al mínimo.
    while(true){
        dyn_sensor_read_center(3, &center); //hace que se de la vuelta entera ya que tarda.
        if(center<min+5){
            dyn_move(DYN_ID_MOTORL,0,false); //Dejámos de girar.
            return; //Salimos.
        }
    }
}

//Función que busca una pared.
void findWall(){
    uint8_t center;
    uint8_t right;
    uint8_t left;

    findBestDirection();
    acelerate(true,10);
    printf("\nMOVING TO A WALL\n");
    //Vamos en linea recta hasta encontrar una pared.
    while(1){
        dyn_sensor_read_center(3,&center);
        printf("\nDISTANCE CENTER: %d",center );
        if(center<11){
            return;
        }
    }
}

//Función que manipula en general el movimiento del robot.
void move(){
    printf("\n\n\nMOVEMENT STARTING");
    uint8_t center;
    uint8_t right;
    uint8_t left;
    uint8_t centerAnt;
    uint8_t rightAnt;
    uint8_t leftAnt;

    uint16_t motorl;
    uint16_t motorr;
    bool dirr;
    bool dirl;

    //Comprovamos que este quieto.
    dyn_move(DYN_ID_MOTORL,0,false);
    dyn_move(DYN_ID_MOTORR,0,false);

    //Buscamos una pared.
    findWall();

    //Para la maniobra de adherencia del inicio.
    bool start = true;
    subSpeed(DYN_ID_MOTORR);
    subSpeed(DYN_ID_MOTORR);

    bool willsmith = true;
    printf("\nJOINING THE WALL");
    while(willsmith) {
        //Primero leemos el estado actual del robot
        dyn_motor_read_speed(DYN_ID_MOTORL, &motorl, &dirl);
        dyn_motor_read_speed(DYN_ID_MOTORR, &motorr, &dirr);
        dyn_sensor_read_center(3, &center);
        dyn_sensor_read_left(3, &left);
        dyn_sensor_read_right(3, &right);
        printf("\n\n\nMOTORS MOVING AT |  LEFT   %u  | i |  RIGHT   %u  |\nOBJECTS AT |  LEFT   %u  | |  CENTER   %u  | |  RIGHT   %u  |", motorl, motorr,left, center, right);

        if(start){
            if(left<9){
                addSpeed(DYN_ID_MOTORR);
                addSpeed(DYN_ID_MOTORR);
                start = false;
            }
            printf("\nWALL JOINED. STARTED TO FOLLOWING");
        }else{
            //Sirve para ir adheriendose a una pared.
            adhere(center,right,left,centerAnt,rightAnt,leftAnt,motorl,motorr);
            centerAnt=center;
            leftAnt=left;
            rightAnt=right;
        }
    }

}

