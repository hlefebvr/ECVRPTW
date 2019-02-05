# Electrical Vehicule Routing Problem with time windows and capacitated stations Metaheuristic

Project developed for a course on "Methods and Models for Decision Support" at the University of Genova.

## Problem statement

Finds feasible solutions in reasonnable time to the EVRPTW problem.
This problem is defined as a vehicle routing problem in which the following constraints hold :
- Single depot
- Time windows [release date, deadline] and service times
- Capacitated vehicles
- Electrical vehicles implying detours to be planned to charging stations
- Capacitated charging stations (limited number of plugs)
- FIFO recharging stations
- Maximum battery capacity

Objective : minimizing the overall travelled distance

## Implemented solver

- Simulated Annealing generating relaxed solutions with respect to the battery constraint (one-point move and 2-OPT move)
- Local search for charging detours insertion
- see `slides/main.pdf` for more details

![Implemented solver](slides/metaheuristic.png)

## Usage

Run the following command to launch the solver on a specific instance file : 
```
./ECVRPTW <instance_file> <max_execution_time>
```

The solver will repeat at least 5 times and untill it founds at least one feasible solution to the problem instance. The average objective value will be logged in the `output.csv` file while the solution will be stored in a file `<instance_filename>_Result.csv`. 

## Example

### Instance file

```
NODES 
   Id           x             y            Type 
    0        0.00          0.00               D 
    1       17.16         42.05               C 
    2       17.92         36.11               C 
    3       14.61         36.93               C 
    4        8.83         34.33               C 
    5       14.42         41.06               C 
    6        7.15         42.29               C 
    7       11.33         42.67               C 
    8       14.59         38.23               C 
    9       17.92         38.74               C 
   10       16.39         33.11               C 
   11       12.31         42.50               C 
   12       15.71         36.88               C 
   13       11.46         33.82               C 
   14       18.40         42.50               C 
   15       18.60         40.78               C 
   16       42.28         13.87               C 
   17       34.81         13.43               C 
   18       39.67         10.33               C 
   19       39.55         11.96               C 
   20       32.79         15.17               C 
   21       36.34         17.31               C 
   22       34.69         15.26               C 
   23       40.21          6.48               C 
   24       34.79         14.68               C 
   25       42.45         11.73               C 
   26       41.58         11.72               C 
   27       36.13          7.71               C 
   28       37.47         16.43               C 
   29       39.94         10.31               C 
   30       41.68          9.33               C 
   31       12.50         25.00               S 
   32       25.00         12.50               S 

RECHARGERS 
   Id      NumRec 
    0           4 
   31           1 
   32           1 

CUSTOMERS 
   Id           Q            ST               R                 D 
    1          36          0.29            7.62             13.92 
    2          38          0.46            2.23              6.04 
    3          43          0.34           11.17             18.02 
    4          46          0.46           13.74             22.01 
    5          31          0.25           10.16             17.32 
    6          22          0.24            3.66              6.50 
    7          42          0.46           12.38             21.10 
    8          30          0.27            7.41             12.23 
    9          46          0.51            9.69             15.89 
   10          31          0.31           11.92             19.95 
   11          45          0.54            4.93             10.23 
   12          25          0.25           12.38             21.35 
   13          24          0.29            2.10              4.51 
   14          47          0.47           11.87             20.19 
   15          21          0.19           12.67             21.00 
   16          41          0.41            7.45             13.39 
   17          42          0.38           13.75             22.96 
   18          33          0.40            8.85             14.87 
   19          31          0.31           11.53             19.06 
   20          50          0.45            9.45             16.96 
   21          32          0.32           13.57             23.15 
   22          33          0.36           14.98             25.10 
   23          24          0.24            1.24              2.87 
   24          30          0.24            2.43              5.17 
   25          29          0.35            1.65              4.32 
   26          47          0.47            6.20             11.38 
   27          27          0.24           14.92             23.97 
   28          29          0.26            6.20             12.01 
   29          32          0.29            6.23             10.61 
   30          41          0.41            8.62             15.65 

INSTDATA 
 Tmax        NumV      Capacity Speed        DistCost  Batt    Ro Alpha 
   33           4          420    50                5    14   0.2  2.00

```

### Solution file

```
Obj;60
Time;2125.27
Vehicle;Orig;Dest;Dist;Rel;Dead;ServTime;DemDest;ArrTimeDest;ArrBattDest;ArrLoadDest;RecTime;PlugUsed;StartRecharging
1;0;13;35.7089;2.1;4.51;0.29;24;2.1;6.85822;184;0;0;0
1;13;2;6.85388;2.23;6.04;0.46;38;2.52708;5.48745;160;0;0;0
1;2;9;2.63;9.69;15.89;0.51;46;9.69;4.96145;122;0;0;0
1;9;8;3.36883;7.41;12.23;0.27;30;10.2674;4.28768;76;0;0;0
1;8;4;6.95612;13.74;22.01;0.46;46;13.74;2.89646;46;0;0;0
1;4;31;10.0259;0;0;0;0;14.4005;0.891287;0;2.34944;1;14.4005
1;31;0;27.9508;0;0;0;0;17.309;1e-09;0;0;0;0
2;0;31;27.9508;0;0;0;0;0.559017;8.40983;343;1.55976;1;0.559017
2;31;6;18.0988;3.66;6.5;0.24;22;3.66;7.9096;343;0;0;0
2;6;11;5.16427;4.93;10.23;0.54;45;4.93;6.87674;343;0;0;0
2;11;5;2.55454;10.16;17.32;0.25;31;10.16;6.36583;298;0;0;0
2;5;15;4.18937;12.67;21;0.19;21;12.67;5.52796;267;0;0;0
2;15;14;1.73159;11.87;20.19;0.47;47;12.8946;5.18164;246;0;0;0
2;14;1;1.31913;7.62;13.92;0.29;36;13.391;4.91782;199;0;0;0
2;1;7;5.86287;12.38;21.1;0.46;42;13.7983;3.74524;163;0;0;0
2;7;3;6.61105;11.17;18.02;0.34;43;14.3905;2.42303;121;0;0;0
2;3;31;12.1152;0;0;0;0;14.9728;9.99999e-10;78;2.51315;1;16.75
2;31;12;12.306;12.38;21.35;0.25;25;19.5092;2.5651;78;0;0;0
2;12;10;3.83084;11.92;19.95;0.31;31;19.8359;1.79894;78;0;0;0
2;10;31;8.99468;0;0;0;0;20.3257;9.99999e-10;47;2.79509;1;20.3257
2;31;0;27.9508;0;0;0;0;23.6798;9.99997e-10;47;0;0;0
3;0;23;40.7288;1.24;2.87;0.24;24;1.24;5.85424;188;0;0;0
3;23;30;3.20677;8.62;15.65;0.41;41;8.62;5.21289;164;0;0;0
3;30;29;1.997;6.23;10.61;0.29;32;9.06994;4.81349;123;0;0;0
3;29;18;0.27074;8.85;14.87;0.4;33;9.36535;4.75934;91;0;0;0
3;18;19;1.63441;11.53;19.06;0.31;31;11.53;4.43246;58;0;0;0
3;19;27;5.45517;14.92;23.97;0.24;27;14.92;3.34142;27;0;0;0
3;27;32;12.117;0;0;0;0;15.4023;0.918028;0;2.33607;1;15.4023
3;32;0;27.9508;0;0;0;0;18.2974;1e-09;0;0;0;0
4;0;32;27.9508;0;0;0;0;0.559017;8.40983;333;0.156767;1;0.559017
4;32;24;10.0298;2.43;5.17;0.24;30;2.43;6.71741;333;0;0;0
4;24;25;8.20842;1.65;4.32;0.35;29;2.83417;5.07572;333;0;0;0
4;25;26;0.870057;6.2;11.38;0.47;47;6.2;4.90171;304;0;0;0
4;26;16;2.26108;7.45;13.39;0.41;41;7.45;4.4495;257;0;0;0
4;16;28;5.44883;6.2;12.01;0.26;29;7.96898;3.35973;216;0;0;0
4;28;21;1.43224;13.57;23.15;0.32;32;13.57;3.07328;187;0;0;0
4;21;22;2.63154;14.98;25.1;0.36;33;14.98;2.54698;155;0;0;0
4;22;17;1.83393;13.75;22.96;0.38;42;15.3767;2.18019;122;0;0;0
4;17;20;2.66608;9.45;16.96;0.45;50;15.81;1.64697;80;0;0;0
4;20;32;8.23486;0;0;0;0;16.4247;9.99999e-10;30;2.79509;1;17.7384
4;32;0;27.9508;0;0;0;0;21.0925;9.99997e-10;30;0;0;0

```
