PUSH
LOAD V0
STACKW 0
PUSH
LOAD V1
STACKW 0
READ T0
LOAD T0
STACKW 0
LOAD 2
STORE T1
STACKR 0
DIV T1
STACKW 1
STACKR 1
STORE T2
WRITE T2
STACKR 0
STORE T3
WRITE T3
L0: NOOP
STACKR 0
STORE T4
STACKR 1
SUB T4
BRZPOS L1
LOAD 1
STORE T5
STACKR 1
ADD T5
STACKW 1
STACKR 1
STORE T6
WRITE T6
BR L0
L1: NOOP
POP
POP
STOP 
V0 1
V1 2
T0 0
T1 0
T2 0
T3 0
T4 0
T5 0
T6 0
