push 0
pop [0]
push 0
pop [1]


push 2
pop rfx
call :main
hlt


main:

in
pop [rfx + 0]
in
pop [rfx + 1]
in
pop [rfx + 2]


push [rfx + 2]

push [rfx + 1]

push [rfx + 0]


push 4
push rfx
add
pop rfx

call :Quadratic

push rfx
push 4
sub
pop rfx


pop [rfx + 3]

If_begin0:
push [rfx + 3]
push 2
jne :If_end0

push [rfx + 3]
out
push [0]
out
push [1]
out

If_end0:


If_begin1:
push [rfx + 3]
push 1
jne :If_end1

push [rfx + 3]
out
push [0]
out

If_end1:


If_begin2:
push [rfx + 3]
push 0
jne :If_end2

push [rfx + 3]
out

If_end2:


If_begin3:
push [rfx + 3]
push -1
jne :If_end3

push [rfx + 3]
out

If_end3:

ret


Quadratic:
pop [rfx + 0]
pop [rfx + 1]
pop [rfx + 2]


If_begin4:


push 0

push [rfx + 0]


push 4
push rfx
add
pop rfx

call :Is_equal

push rfx
push 4
sub
pop rfx


push 1
jne :If_end4



push [rfx + 2]

push [rfx + 1]


push 4
push rfx
add
pop rfx

call :Line

push rfx
push 4
sub
pop rfx


ret

If_end4:

push [rfx + 1]
push 2
POW
push 4
push [rfx + 0]
MUL
push [rfx + 2]
MUL
SUB
pop [rfx + 3]

If_begin5:
push [rfx + 3]
push 0
jbe :If_end5

push [rfx + 1]
SUB
push [rfx + 3]

SQRT
ADD
push 2
DIV
push [rfx + 0]
DIV
pop [0]
push [rfx + 1]
SUB
push [rfx + 3]

SQRT
SUB
push 2
DIV
push [rfx + 0]
DIV
pop [1]
push 2
ret

If_end5:


If_begin6:


push 0

push [rfx + 3]


push 4
push rfx
add
pop rfx

call :Is_equal

push rfx
push 4
sub
pop rfx


push 1
jne :If_end6

push [rfx + 1]
push 2
DIV
push [rfx + 0]
DIV
SUB
pop [0]
push 1
ret

If_end6:


If_begin7:
push [rfx + 3]
push 0
jae :If_end7

push 0
ret

If_end7:

ret


Is_equal:
pop [rfx + 0]
pop [rfx + 1]


If_begin8:
push [rfx + 0]
push [rfx + 1]
SUB
push 1e-06
jae :If_end8


If_begin8:
push [rfx + 0]
push [rfx + 1]
SUB
push -1e-06
jbe :If_end8

push 1
ret

If_end8:


If_end9:

push 0
ret
ret


Line:
pop [rfx + 0]
pop [rfx + 1]


If_begin10:


push 0

push [rfx + 0]


push 2
push rfx
add
pop rfx

call :Is_equal

push rfx
push 2
sub
pop rfx


push 1
jne :If_end10


If_begin10:


push 0

push [rfx + 1]


push 2
push rfx
add
pop rfx

call :Is_equal

push rfx
push 2
sub
pop rfx


push 1
jne :If_end10

push -1
ret

If_end10:

push 0
ret

If_end11:


If_begin12:


push 0

push [rfx + 1]


push 2
push rfx
add
pop rfx

call :Is_equal

push rfx
push 2
sub
pop rfx


push 1
jne :If_end12

push [rfx + 1]
push [rfx + 0]
DIV
SUB
pop [0]
push 1
ret

If_end12:

push 0
pop [0]
push 1
ret
ret


