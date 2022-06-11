.data
#variables declaration
	Farray: .float 1.0 , -2.0 , 3.0 , 4.0 , -5.0 ,10.0 , -20.0 , 30.0 , 40.0 , 50.0
	Size: .word 10
	MS1: .asciiz " Average of all spendings = "
	MS2: .asciiz " Net income = "
	MS3: .asciiz " Maximum earning value = "
	MS4: .asciiz "no maximum earning value ( all are spending )"
	newline: .asciiz "\n"	
.text
.globl main
#main start		
main:
	#passing parameters
	la $a0,Farray
	lw $a1,Size
	jal  average
	
	#passing parameters
	la $a0,Farray
	lw $a1,Size
	jal max
	
	mtc1 $zero, $f1
	c.eq.s $f0, $f1
	bc1f  printMax
	
	#case 1 : no max number has been found
	noMax:
 	la $a0,MS4
	li $v0, 4 
	syscall 

	la $a0,newline
	li $v0, 4 
	syscall
	j  endPrintMax
	
	#case 2 : max number has been found
	printMax:
 	la $a0,MS3
	li $v0, 4 
	syscall 
	
	mov.s $f12,$f0 
	li $v0,2
	syscall 

	la $a0,newline
	li $v0, 4 
	syscall 
	endPrintMax:

li  $v0,10
syscall 
#main end

#########################################################################
#functions
average: 
#This procedure will return the average of all elements and the net income for the array of size 10 .
	addi $t1,$a0,0
	addi $t5,$zero,4
	mult $a1,$t5
	mflo $t6
	add $t4,$t6,$a0
	
	addi $t2,$0,0 #counter = 0
	mtc1 $zero, $f1
	mtc1 $zero, $f3
	mtc1 $zero, $f6
NextNumber: 
	beq $t1,$t4,FinshSum

	lw $t0,0($t1) 
	mtc1 $t0,$f0 
	addi $t1,$t1,4 #f0 = A[i++]
	
	add.s $f3,$f0,$f3 # f3 net sum
	c.le.s $f0, $f6
	bc1f  NextNumber
	nop
	add.s $f1,$f0,$f1# negetve sum
	addi $t2,$t2,1
	j NextNumber
FinshSum: 
	addi $t0,$t2,0
	mtc1 $t0,$f4 
	cvt.s.w $f5, $f4 # coubter value at f5
	
	la $a0,MS2
	li $v0, 4 
	syscall 
	
	mov.s $f12,$f3 
	li $v0,2
	syscall 
	
	la $a0,newline
	li $v0, 4 
	syscall 
	
	div.s $f1, $f1, $f5
	
	la $a0,MS1
	li $v0, 4 
	syscall 
	
	mov.s $f12,$f1 
	li $v0,2
	syscall 

	la $a0,newline
	li $v0, 4 
	syscall 
	
	jr $ra  
#######################
max: 
#This procedure will return the greatest value from floating array at reg $f0
	addi $t1,$a0,0
	addi $t5,$zero,4
	mult $a1,$t5
	mflo $a1
	add $t4,$a1,$a0
	
	addi $t6,$0,0
	mtc1 $zero, $f0
	mtc1 $zero, $f2

nextValue:
	
	beq $t1,$t4,FinshMax
	
	lw $t0,0($t1)
	mtc1 $t0,$f1 
	addi $t1,$t1,4
	
	c.le.s $f1, $f0
	bc1t nextValue
	nop
	
	c.le.s $f2, $f1
	bc1t SetMax
	nop
	
	j nextValue
FinshMax:
	jr $ra # return 
SetMax:
	mov.s $f0, $f1
	j nextValue
	
	jr $ra # return 
#######################					
#end functions
