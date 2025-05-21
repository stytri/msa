
	mov sp 077770               ; Stack base
	mov r5 $1                   ; set return address
	jmp main                    ; call main
$1:
	mov r2 077770               ; channel bank base
$2:
		lda r1 r2 srz           ; read channel pending, until zero
		jmp $2                  ; loop back

	mov r0 1, sta r0 r2         ; halt

\0x100:

putc:
	sta r1 --sp                 ; push r1
	sta r2 --sp                 ; push r2
	sta r3 --sp                 ; push r3

	mov r2 077770               ; channel bank base
putc$1:
		lda r1 r2               ; read output pending
		and r1 2 srz            ; is channel #1 pending bit clear
		jmp putc$1              ; loop back

	mov r3 1, add r3 r2         ; channel 1 buffer
	sta r0 r3                   ; write r0 to output buffer
	lda r1 r2                   ; read channel pending
	ior r1 2, sta r1 r2         ; set channel #1 pending bit

	lda r3 sp++                 ; pop r2
	lda r2 sp++                 ; pop r2
	lda r1 sp++                 ; pop r1
	mov pc r5                   ; return

puts:
	sta r5 --sp                 ; push r5 (return address)
	sta r1 --sp                 ; push r1

	mov r1 r0                   ; r1 = r0
	mov r5 puts$2               ; set return address
	mov pc r5                   ; jump to puts$2
puts$1:
		jmp putc                ; call putc
puts$2:
		lda r0 r1++ srz         ; read character until nul
		jmp puts$1              ; loop back

	lda r1 sp++                 ; pop r2
	lda r5 sp++                 ; pop r5
	mov pc r5                   ; return

\0x100:

main:
	sta r5 --sp                 ; push r5 (return address)

	mov r0 Hello$               ; set string to print
	mov r5 main$1               ; set return address
	jmp puts                    ; call puts
main$1:
	lda r5 sp++                 ; pop r5
	mov pc r5                   ; return

\0x100:

Hello$:
	'H' 'e' 'l' 'l' 'o' ' '
	'W' 'o' 'r' 'l' 'd'	'\n'
	0
