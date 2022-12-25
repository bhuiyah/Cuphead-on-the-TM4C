; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

FP RN 11
COUNT EQU 0
NUM EQU 4  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; R0=0,    then output "0"
; R0=3,    then output "3"
; R0=89,   then output "89"
; R0=123,  then output "123"
; R0=9999, then output "9999"
; R0=4294967295, then output "4294967295"
LCD_OutDec
		PUSH {R4-R9, R11, LR}
		SUB SP, #8 ;Allocating
		MOV FP, SP
		CMP R0, #0
		BEQ Zero
		STR R0,[FP, #NUM]
		MOV R4, #0
		STR R4,[FP, #COUNT]
		MOV R5, #10 
LOOP
		LDR R4, [FP, #NUM] ;R4 holds NUM
		CMP R4, #0
		BEQ NEXT
		UDIV R6, R4, R5 ;R6 is the quotient
		MUL R7, R6, R5  ;R7 (10*Q) which is used to find the remainder
		SUB R8, R4, R7 ;R8 is the remainder
		PUSH {R8}
		STR R6, [FP, #NUM]
		LDR R6, [FP, #COUNT]
		ADD R6, #1
		STR R6, [FP, #COUNT]
		B LOOP
NEXT
		LDR R4, [FP, #COUNT]
		CMP R4, #0
		BEQ DONE
		POP {R0}
		ADD R0, #0x30 ;Changes NUM to ASCII
		BL ST7735_OutChar
		SUB R4, #1
		STR R4, [FP, #COUNT]
		B NEXT
DONE 
		ADD SP, #8
		POP {R4-R9, R11, LR}
		BX  LR
		
Zero
		MOV R0, #0x30
		BL ST7735_OutChar
		B DONE

;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000"
;       R0=3,    then output "0.003"
;       R0=89,   then output "0.089"
;       R0=123,  then output "0.123"
;       R0=9999, then output "9.999"
;       R0>9999, then output "*.***"
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
		PUSH {R4-R9, R11, LR}
		SUB SP, #8 ;Allocating
		MOV FP, SP
		CMP R0, #0
		BEQ Zero1
		STR R0,[FP, #NUM]
		MOV R4, #0
		STR R4,[FP, #COUNT]
		MOV R5, #10 
LOOP1
		LDR R4, [FP, #NUM] ;R4 holds NUM
		CMP R4, #0
		BEQ LOOP2
		UDIV R6, R4, R5 ;R6 is the quotient
		MUL R7, R6, R5  ;R7 (10*Q) which is used to find the remainder
		SUB R8, R4, R7 ;R8 is the remainder
		PUSH {R8}
		STR R6, [FP, #NUM]
		LDR R6, [FP, #COUNT]
		ADD R6, #1
		STR R6, [FP, #COUNT]
		B LOOP1
LOOP2		
		LDR R4, [FP, #COUNT]
		MOV R9, #1 ;Decimal counter
		MOV R6, #5 ; Total Char counter
		MOV R5, #4 ; Zero counter
		CMP R6, R4
		BLS Large5
		SUB R5, R4
		CMP R5, #0
		BEQ DIG4
LOOP3
		CMP R9, #0
		BEQ PERIOD
		MOV R0, #0x30
		BL ST7735_OutChar
		SUB R9, #1
		SUB R5, #1
		CMP R5, #0
		BNE LOOP3
DIG4
		LDR R4, [FP, #COUNT]
		CMP R9, #0
		BEQ PERIOD2
		CMP R4, #0
		BEQ DONE1
		POP {R0}
		ADD R0, #0x30 ;Changes NUM to ASCII
		BL ST7735_OutChar
		SUB R9, #1
		SUB R4, #1
		STR R4, [FP, #COUNT]
		B DIG4
DONE1
		ADD SP, #8
		POP {R4-R9, R11, LR}
		BX  LR
		
Large5
		POP {R0}
		SUBS R4, #1
		BNE Large5
		MOV R0, #0x2A
		BL ST7735_OutChar
		MOV R0, #0x2E
		BL ST7735_OutChar
		MOV R0, #0x2A
		BL ST7735_OutChar
		MOV R0, #0x2A
		BL ST7735_OutChar
		MOV R0, #0x2A
		BL ST7735_OutChar
		B DONE1
		
PERIOD
		MOV R0, #0x2E
		BL ST7735_OutChar
		SUB R9, #1
		B LOOP3
PERIOD2
		MOV R0, #0x2E
		BL ST7735_OutChar
		SUB R9, #1
		B DIG4
Zero1
		MOV R0, #0x30
		BL ST7735_OutChar
		MOV R0, #0x2E
		BL ST7735_OutChar
		MOV R0, #0x30
		BL ST7735_OutChar
		MOV R0, #0x30
		BL ST7735_OutChar
		MOV R0, #0x30
		BL ST7735_OutChar
		B DONE1
		
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
