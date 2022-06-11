.data
#variables declaration
 	newline: .asciiz "\n"
 	MSG1: .asciiz " Enter floating number  : "
 	MSG2: .asciiz " Sqrt( "
 	MSG3: .asciiz " ) = "
 	MSG4: .asciiz " pleas enter positive number "
 	tolerance: .float 0.00001
.text
.globl main
# MAIN PROGRAM
 main:
 	#print("Enter floating number  : ");
	la $a0,MSG1
	li $v0,4 
	syscall 
 	
 	#sacanf("%f",&__f0);
	li $v0,6 
	syscall #f0 = N
	l.s $f1,tolerance
	mov.s $f2,$f0	
	#if user enter negative number keep reading 
	#if f2 < 0 jmp to main
	mtc1 $zero, $f10
	c.le.s $f2,$f10
	bc1f  cont # if (|x(i+1)-x(i)|<tolerunce)go to endLoop
	#print(" pleas enter positive number ");
	la $a0,MSG4
	li $v0,4 
	syscall 
	
	la $a0,newline
	li $v0, 4 
	syscall 
	j main	
	
	cont :			
	jal floatSqrt
 
 	la $a0,MSG2
	li $v0,4 
	syscall 
	
 	mov.s $f12,$f2 
	li $v0,2
	syscall 

 	la $a0,MSG3
	li $v0,4 
	syscall 
	
	mov.s $f12,$f0 
	li $v0,2
	syscall 

	la $a0,newline
	li $v0, 4 
	syscall  

ori $v0, $zero, 10 #syscall will exit
syscall #exit
#END MAIN
floatSqrt:
	#push reg f2 - f19
	subi $sp,$sp,72
	s.s $f19,68($sp)
	s.s $f18,64($sp)
	s.s $f17,60($sp)
	s.s $f16,56($sp)
	s.s $f15,52($sp)
	s.s $f14,48($sp) 
	s.s $f13,44($sp)
	s.s $f12,40($sp)
	s.s $f11,36($sp)
	s.s $f10,32($sp)
	s.s $f9,28($sp)
	s.s $f8,24($sp)
	s.s $f7,20($sp)
	s.s $f6,16($sp)
	s.s $f5,12($sp)
	s.s $f4,8($sp)
	s.s $f3,4($sp)
	s.s $f2,($sp)
		
	mov.s $f12,$f0#move N from f0 to f12 // ANS=N 
	mtc1 $zero, $f13#set f13 value to zero // temp=0
	
	addi $t0,$0,2
	mtc1 $t0,$f5
	cvt.s.w $f14, $f5#set f14 value to 2.0 
	
	Loop:
	mov.s $f13,$f12 #temp = ANS
	div.s $f4,$f0,$f12#f4 = N / ANS
	add.s $f12,$f12,$f4#Ans = ANS + N / ANS
	div.s $f12,$f12,$f14#Ans = Ans/2.0

	sub.s $f9,$f13,$f12
	abs.s $f11,$f9
	c.le.s $f11,$f1
	bc1t  endLoop # if (|x(i+1)-x(i)|<tolerunce)go to endLoop

	j Loop
	
	endLoop:
	mov.s $f0,$f12
	
	#pop reg f2 - f19	
	l.s $f2,($sp)
	l.s $f3,4($sp)
	l.s $f4,8($sp)
	l.s $f5,12($sp)
	l.s $f6,16($sp)
	l.s $f7,20($sp)
	l.s $f8,24($sp)
	l.s $f9,28($sp)
	l.s $f10,32($sp)
	l.s $f11,36($sp)
	l.s $f12,40($sp)
	l.s $f13,44($sp)
	l.s $f14,48($sp)
	l.s $f15,52($sp)
	l.s $f16,56($sp)
	l.s $f17,60($sp)
	l.s $f18,64($sp)
	l.s $f19,68($sp)
	addi $sp,$sp,72
	
jr $ra # return 
