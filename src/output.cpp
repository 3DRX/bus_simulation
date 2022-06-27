#include "core.h"
#include "input.h"
#include "main.h"
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>

extern int TIME;
extern CAR car;
extern ENVIRONMENT env;
extern STATION station;
extern GLOB global;

void printLines(void)
{
    // fprintf( env.output,"TIME:%d\n", TIME );
    // fprintf(env.output, "BUS:\n" );
    // fprintf(env.output, "position:%d\n", car.position );
    printf("TIME:%d\n", TIME);
    printf("BUS:\n");
    printf("position:%d\n", car.position);
    car.target[0][20] = -1; // 防越界
    // fprintf(env.output, "target:" );
    printf("target:");
    for (int i = 0; i < 20; i++) {
        if (car.target[0][i] == -1) {
            break;
        }
        // fprintf(env.output, "%d", car.target[ 0 ][ i ] );
        printf("%d", car.target[0][i]);
    }
    // fprintf(env.output, "\n" );
    // fprintf(env.output, "STATION:\n" );
    printf("\n");
    printf("STATION:\n");
    station.clockwise[0][20] = -1; // 防越界
    station.counterclockwise[0][20] = -1; // 防越界
    // fprintf(env.output, "clockwise:" );
    printf("clockwise:");
    for (int i = 0; i < 20; i++) {
        if (station.clockwise[0][i] == -1) {
            break;
        }
        // fprintf(env.output, "%d", station.clockwise[ 0 ][ i ] );
        printf("%d", station.clockwise[0][i]);
    }
    // fprintf(env.output, "\n" );
    // fprintf(env.output, "counterclockwise:" );
    printf("\n");
    printf("counterclockwise:");
    for (int i = 0; i < 20; i++) {
        if (station.counterclockwise[0][i] == -1) {
            break;
        }
        // fprintf(env.output, "%d", station.counterclockwise[ 0 ][ i ] );
        printf("%d", station.counterclockwise[0][i]);
    }
    // fprintf(env.output, "\n" );
    printf("\n");
}

void outPut()
{
    if (TIME == -1) {
        // fprintf(env.output, "end\n" );
        printf("end\n");
        if (env.STRATEGY == ENVIRONMENT::FCFS) {
            FCFS_freeList(env.headnode);
        }
        exit(EXIT_SUCCESS);
        // TODO
    } else {
        static auto last_car_pos = car.position;
        static auto last_car_state = global.car_state;
        if (global.glob_state_refresh == true) {
            if (car.position != last_car_pos || global.car_state != last_car_state) {
                last_car_pos = car.position;
                last_car_state = global.car_state;
                global.glob_state_refresh = false;
                printLines();
            }
        }
    }
}
