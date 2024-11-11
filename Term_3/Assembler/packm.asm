clear_screen MACRO
    ; set video mode:
    ; text mode. 80x25. 16 colors. 8 pages.         
	push ax               
	mov ax, 3h         
	int 10h              
	pop ax                
ENDM

SetNewChar macro Replace, Symb     
    push    ES          ;was in example
    push    BP          ;
    mov     ax, @data
    mov     ES, ax
               
    xor     bl, bl
    mov     bh, 16              ;amount of rows
    mov     cx, offset Symb
    mov     bp, cx              ;put offset to symbol
    mov     dx, Replace         ;put symbol index, wich you want to replace
    mov     cx, 1               ;amount of symbols to replace
    mov     ax, 1100h           ;function of int 10h
    int     10h
    
    pop     BP          ;was in example
    pop     ES          ;
endm    
 
 
print_sym MACRO x_cursor, y_cursor, ascii_symb, atribute    
    mov     dl, x_cursor
    mov     dh, y_cursor
    mov     bl, atribute
    mov     ah, 02h
    int     10h
    
    mov bh, 0
    mov al, ascii_symb  
    mov cx, 1              
    mov ah, 09h
    int     10h 
ENDM

set_cursor MACRO x_cursor, y_cursor
    push ax
    push dx
       
    mov     dl, x_cursor
    mov     dh, y_cursor
    mov     ah, 02h
    int     10h
    
    pop dx
    pop ax
ENDM

;ZF = 1 - buff is empty
;AH = scan-code             
check_buffer MACRO          ; check on entered symbol
	mov ah, 01h              
	int 16h    ; al - ascii or 0 or prefix scan code         
ENDM           ; ah - scan code or maden bigger ascii           
                           
read_from_buffer MACRO     ; read entered key
	mov ah, 00h            ; if ascii - ah:ascii, al:scan code
	int 16h                ; expoled ascii:
ENDM                       ; al: prefix or 0, ah: exploded ascii  
                           
;result in cx:dx          
get_timer_value MACRO      ; get time
	push ax                                  
	mov ax, 00h             
	int 1Ah                                           
	pop ax                  
ENDM                        

; this macro prints a char in AL and advances
; the current cursor position:
PUTC    MACRO   char
        PUSH    AX
        MOV     AL, char
        MOV     AH, 0Eh
        INT     10h     
        POP     AX
ENDM
;end macro help
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.model	small
.stack	100h

.data            
    gameBoard   db "11111111111111111111111111111111111111111111111111111111111111111111111111111111"           
                db "110. . . . . . 8 . . . . . . . . . . . . . . . . . 8 . . . . . . . 8 . . . . 011"
                db "110.011111111111110.01111111111111110.0110.01111111111111110.011111111111110.011"
                db "110.011111111111110. . . . . . . . . .0110. . . . . . . 0110.011111111111110.011"
                db "   .011111111111110.0110.011111111110.0110.011111111110.0110.011111111111110.   "
                db "110.011111111111110.0110. . . . 01110.0110. . . . .0110.0110.011111111111110.011"
                db "110.011111111111110.0110.011110.01110.0110.0111110.0110.0110.011111111111110.011"
                db "110.011111111111110.0110 . . . . . . .0110. . . . . . . 0110.011111111111110.011"
                db "110. . . . . . . . .0110.011110.011111111111110.0111110.0110. . . . . . . . .011" 
                db "110.011111111111110.0110.011110. . . . . . . . .0111110.0110.011111111111110.011"
                db "110. . . . . . . . .0110.011110.0111110 0111110.0111110.0110. . . . . . . . .011"                
                db "   .0111110.0111110.0110.011110.               .0111110.0110.0111110.0111110.   "
                db "110. . . . . . . . .0110.011110.0111110 0111110.0111110.0110. . . 8 . . . . .011"
                db "110.011111111111110.0110. . . 8 . . . . . . . . . . . . 0110.011111111111110.011"
                db "1108 . . . . . . . .0110.01111111110.0111110.01110.0110.0110. . . . . . . . .011"
                db "110.011111111111110.0110.01111111110.0111110.01110.0110.0110.011111111111110.011"
                db "110.011111111111110.0110. . . . . . . . . . .01110.0110.0110.011111111111110.011"
                db "110.011111111111110.0110.01110.01110.0111110. . . 80110.0110.011111111111110.011" 
                db "   .011111111111110.0110.01110.01110.0111110.0111111110.0110.011111111111110.   "
                db "110.011111111111110. . . . . . . . . . . . . . . . . . .0110.011111111111110.011"
                db "110.011111111111110.0111111111111111110.01111111111111111110.011111111111110.011"
                db "110. . . . . . . . . . . . . . . 8 . . . . . . . . . . . . . . . . . . . . . 011"
                db "11111111111111111111111111111111111111111111111111111111111111111111111111111111"
    
    
    PacMRight db 00000000b ;00h  
              db 00000000b ;00h 
              db 00000000b ;00h
              db 00000000b ;1Ch
              db 00011110b ;3Eh
              db 00111111b ;67h
              db 01101100b ;6Ch
              db 01111000b ;78h
              db 01110000b ;70h
              db 01111000b ;78h
              db 00111100b ;7Ch
              db 00011111b ;3Fh
              db 00000000b ;1Eh
              db 00000000b ;00h
              db 00000000b ;00h
              db 00000000b ;00h 
                  
    PacMLeft  db 00000000b ;00h  
              db 00000000b ;00h 
              db 00000000b ;00h
              db 00000000b ;1Ch
              db 01111000b ;3Eh
              db 11111100b ;67h
              db 00110110b ;6Ch
              db 00011110b ;78h
              db 00001110b ;70h
              db 00011110b ;78h
              db 00111100b ;7Ch
              db 11111000b ;3Fh
              db 00000000b ;1Eh
              db 00000000b ;00h
              db 00000000b ;00h
              db 00000000b ;00h              
              
    PacMUP    db 00000000b ;00h  
              db 00000000b ;00h 
              db 00000000b ;00h
              db 00000000b ;1Ch
              db 01000001b ;3Eh
              db 11000001b ;67h
              db 11100011b ;6Ch
              db 11110111b ;78h
              db 11011111b ;70h
              db 01111110b ;78h
              db 00111100b ;7Ch
              db 00000000b ;3Fh
              db 00000000b ;1Eh
              db 00000000b ;00h
              db 00000000b ;00h
              db 00000000b ;00h    
           
    PacMDown  db 00000000b ;00h  
              db 00000000b ;00h 
              db 00000000b ;00h
              db 00000000b ;1Ch
              db 00000000b ;3Eh
              db 00111100b ;67h
              db 01111110b ;6Ch
              db 11111011b ;78h
              db 11101111b ;70h
              db 11000111b ;78h
              db 10000011b ;7Ch
              db 10000010b ;3Fh
              db 00000000b ;1Eh
              db 00000000b ;00h
              db 00000000b ;00h
              db 00000000b ;00h
              
              
    GhostRight db 00000000b ;  
               db 00000000b ; 
               db 00000000b ;
               db 00111100b ;
               db 01111110b ;
               db 01001010b ;
               db 01001010b ;
               db 01111110b ;
               db 01111110b ;
               db 01111110b ;
               db 01111110b ;
               db 01111110b ;
               db 00101010b ;
               db 00000000b ;
               db 00000000b ;
               db 00000000b ;      
       
    pacman_dir_symb db 1 ;2-right 192-left 193-down 194-up   
                               
    x_screen    db 80   
    y_screen    db 23
    
    pacman_x    db 3   
    pacman_y    db 1
    pacman_dir  db 2 ; 0 - stay; 1 - up; 2 - right;
                     ; 3 - down; 4 - left                      
    pacman_prev_x db 3                 
    pacman_prev_y db 1
    enemy1_prev_x    db 34   
    enemy1_prev_y    db 11
    enemy2_prev_x    db 39   
    enemy2_prev_y    db 11                           
    enemy3_prev_x    db 44   
    enemy3_prev_y    db 11    
                                                                                                        
    enemy1_x    db 34   
    enemy1_y    db 11
    enemy2_x    db 39   
    enemy2_y    db 11                           
    enemy3_x    db 44   
    enemy3_y    db 11
      
    enemies_are_helpless db 0 
    
    enemy1_atr           db 00001010b
    enemy2_atr           db 00001101b
    enemy3_atr           db 00001111b 
    
    enemy1_is_alive db 1 
    enemy2_is_alive db 1
    enemy3_is_alive db 1
    
    enemy1_dir  db 1      ;up - 0, right - 1, down - 2, left - 3
    enemy2_dir  db 0
    enemy3_dir  db 3
    
    enemy1_prev_symbol  db 32
    enemy2_prev_symbol  db 32
    enemy3_prev_symbol  db 32
    enemy1_prev_symbatr db 0 
    enemy2_prev_symbatr db 0    
    enemy3_prev_symbatr db 0      
    
    
    enemies_are_helpless_endtime   dw 0
    enemies_are_helpless_fulltime  dw 120
    
    score_num   dw 0
    win_score   dw 3500
        
    pressKeyMsg     db " Enter ESC to escape$"
    scorePrintMsg   db "SCORE: $"   
   winBoard         db   "10000000000000000000000000000000000000000000000000000000000000002"
                    db   "9vv     vv  vvvvvvvv   vvvvv  vvvvvv   vvvvv   vvvvv    vv    vv9"              
                    db   "9 vv   vv      vv     vv        vv    vv   vv  vv  vv    vv  vv 9" 
                    db   "9  vv vv       vv     vv        vv    vv   vv  vvvvv      vvvv  9"   
                    db   "9   vvv        vv     vv        vv    vv   vv  vv  vv      vv   9"
                    db   "9    v      vvvvvvvv   vvvvv    vv     vvvvv   vv   vv     vv   9" 
                    db   "30000000000000000000000000000000000000000000000000000000000000004"                                            
    
    delay_time     dw 3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;                        
.code      
start: 
    ; set segment registers:
    mov ax, @data
    mov ds, ax 
    mov ax, 0B800h           ; video memory start
    mov es, ax                                
	xor ax, ax     
    ; set video mode:
    ; text mode. 80x25. 16 colors. 8 pages.    
    clear_screen
    ; hide text cursor:
    mov     ah, 1
    mov     ch, 2bh
    mov     cl, 0bh
    int     10h       
        
    SetNewChar 1, GhostRight
    SetNewChar 2, PacMRight 
    SetNewChar 192, PacMLeft 
    SetNewChar 193, PacMDown 
    SetNewChar 194, PacMUP 
         
    call print_screen
    call game_process    
    exit:
        clear_screen  
        mov ax, 4c00h ; exit to operating system.
        int 21h
   
;;;;;;;;;procedures;;;;;;;;;;;;;;;;;;;;;;;;;;    
    PRINT_SCREEN proc         
        call print_field     
        call print_pacman 
        call print_enemies             
          
        ret
    print_screen endp
    
    ;;;;;;;;;;;;;;;;
    PRINT_FIELD proc 
        xor     si, si     
        mov     bh, 0   ; num of page
        mov     cx, 1   ; num of symbols repeat      
        mov     dl, 0   ; current column
        mov     dh, 0   ; current row  
        
        mov     si, offset gameBoard                    
        jmp     next_char 
        
    stop_print1:
        jmp far ptr stop_print
        
        next_row:       ; new line
        inc     dh
        cmp     dh, y_screen
        je      stop_print1
        mov     dl, 0
            
        next_char:        
        ;cmp [si], elem               
        mov     ax, [si]
        cmp     al, '1'          
        je is1
        cmp     al, '0'
        je is0
        cmp     al, ' '
        je isS
        cmp     al, '.'
        je isT
        cmp     al, '8'
        je is81
                          
        ; blue walls          
        is1:  
              print_sym dl, dh, 219, 00000001b
              jmp new_elem_print
        ; black space but it is wall              
        is0:
              print_sym dl, dh, 48, 0
              jmp new_elem_print
              is81: 
                jmp far ptr is8      
        ; black space              
        isS:
              print_sym dl, dh, 32, 0
              jmp new_elem_print
        ; yellow dot on black space 
        isT:  
              print_sym dl, dh, 249, 12
              jmp new_elem_print
        ; bonuses on black space 
        is8:  
              print_sym dl, dh, 15, 11
              jmp new_elem_print
                                          
        ; next elem
        new_elem_print:
        inc     si
        inc     dl
        cmp     dl, x_screen   
        je      next_row1
        jmp     next_char  
        
        next_row1:
            jmp far ptr next_row
        
        stop_print:
            ; print score
            set_cursor 40, 24           
            lea     dx, scorePrintMsg
            mov     ah, 9
            int 21h        ; output string at ds:dx       
            ; print any key
            set_cursor 0, 24           
            lea     dx, pressKeyMsg
            mov     ah, 9
            int 21h         ; output string at ds:dx 
            mov di, 819 
            ret                            
    print_field endp
    
    ;;;;;;;;;;;;;;;;;;;;;
    PRINT_PACMAN proc
        print_sym pacman_x, pacman_y, pacman_dir_symb, 14 ;10001110b;
        ret
    print_pacman endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    COMPARE_WITH_TUNNEL macro 
        mov ah, 8       ; compare with space
        int 10h                            
        cmp al, 32               
            je set_random_dir
        cmp al, 15               
            je set_random_dir        
        cmp al, 249              
            je set_random_dir        

    endm 
          
    ;;;;;;;;;;;;;;;;;;;;;;;;;;
    SET_ENEMY_COORDS proc ; coord_x in cl, coord_y in ch, dir in bl
    push ax
    push dx
    mov bh, 0    
    xor ax, ax
    xor dx, dx
        
        cmp bl, 0
    je vertical_dir
        cmp bl, 1
    je horisontal_dir    
        cmp bl, 2
    je vertical_dir
        cmp bl, 3
    je horisontal_dir

    
vertical_dir:
    mov dl, cl 
    dec dl
    set_cursor dl, ch 
        compare_with_tunnel                                   
    mov dl, cl
    inc dl
    set_cursor dl, ch
        compare_with_tunnel       
    jmp ready_to_move
        
horisontal_dir:
    mov dh, ch 
    dec dh
    set_cursor cl, dh
        compare_with_tunnel                        
    mov dh, ch 
    inc dh
    set_cursor cl, dh
        compare_with_tunnel                        
    jmp ready_to_move

far_exit6:
    jmp far ptr far_exit

set_random_dir:
    push ax
    push cx 
        mov ah, 2Ch
        int 21h    
    pop cx
    xor ax, ax
    mov al, dl
    
        xor dx, dx
        push bx
        mov bh, 4
        div bh
        
        pop bx  
        mov bl, ah  
    pop ax
    
ready_to_move:
    cmp cl, pacman_x
        jne continue_enemy_move    
    cmp ch, pacman_y
        jne continue_enemy_move
    cmp enemies_are_helpless, 0        
        je far_exit6
        
            cmp cl, enemy1_x
                jne maybi_enemy2
                cmp ch, enemy1_y
                    jne maybi_enemy2
                        mov enemy1_is_alive, 0 
                        jmp continue_enemy_move
        maybi_enemy2:
            cmp cl, enemy2_x
                jne maybi_enemy3
                cmp ch, enemy2_y
                    jne maybi_enemy3
                        mov enemy2_is_alive, 0
                        jmp continue_enemy_move
        
        maybi_enemy3:
            cmp cl, enemy3_x
            jne continue_enemy_move
                cmp ch, enemy3_y    
                jne continue_enemy_move 
                mov enemy3_is_alive, 0
                               
continue_enemy_move:  
    cmp bl, 0
    je up_direct
    cmp bl, 1
    je right_direct
    cmp bl, 2
    je down_direct
    cmp bl, 3
    je left_direct    
    
up_direct: 
    mov dh, ch
    dec dh
    set_cursor cl, dh
    mov ah, 8
    int 10h 
        cmp al, 219
        je set_random_dir3
        cmp al, 48
        je set_random_dir3        
        cmp al, 1
        je set_random_dir3
        dec ch
        jmp set_e_coords_end
        
right_direct: 
    mov dl, cl
    inc dl
    set_cursor dl, ch
    mov ah, 8
    int 10h 
        cmp al, 48
            je set_random_dir3
        cmp al, 1
            je set_random_dir3        
        inc cl 
        cmp cl, 80 
        jne set_e_coords_end
            mov cl, 0
        jmp set_e_coords_end 
        
        set_random_dir3:
            jmp far ptr set_random_dir
        
down_direct: 
    mov dh, ch           
    inc dh
    set_cursor cl, dh
    mov ah, 8
    int 10h
    cmp al, 219
        je set_random_dir1
    cmp al, 48
        je set_random_dir1         
    cmp al, 1
        je set_random_dir1        
  inc ch
  jmp set_e_coords_end  
  
    set_random_dir1:
        jmp far ptr set_random_dir 
  
left_direct: 
    mov dl, cl
    dec dl
    set_cursor dl, ch
    mov ah, 8
    int 10h 
    cmp al, 48
        je set_random_dir1 
    cmp al, 1
        je set_random_dir1    
        dec cl 
        cmp cl,-1 
        jne set_e_coords_end
        mov cl, 79
        jmp set_e_coords_end


    set_e_coords_end:
        pop dx 
        pop ax
        ret
    set_enemy_coords endp
        
    ;;;;;;;;;;;;;;;;;;;;
    PRINT_ENEMIES proc 
        cmp enemy1_is_alive, 1 
        jne next_enemy_print2
        print_sym enemy1_x, enemy1_y, 1, enemy1_atr
         
   next_enemy_print2:
        cmp enemy2_is_alive, 1 
        jne next_enemy_print3     
        print_sym enemy2_x, enemy2_y, 1, enemy2_atr  
   
   next_enemy_print3:
        cmp enemy3_is_alive, 1 
        jne end_enemy_print     
        print_sym enemy3_x, enemy3_y, 1, enemy3_atr 
   
   end_enemy_print:     
        ret
    print_enemies endp
    
;;;;;;;;;;;;;;;;;;;;;;;;
    GAME_PROCESS proc
        
    main_game_loop:
mov ax, win_score
cmp score_num, ax
jne it_is_not_final_score
call you_won
jmp far_exit1

it_is_not_final_score:
    cmp enemy1_is_alive, 1
je all_enemies_are_alive
    cmp enemy2_is_alive, 1
je all_enemies_are_alive
    cmp enemy3_is_alive, 1 
je all_enemies_are_alive
call you_won    
jmp far_exit1


all_enemies_are_alive:
cmp enemies_are_helpless, 1
jne delay_place
push dx
push cx
   get_timer_value
   cmp dx,enemies_are_helpless_endtime
   jnl again_enemies 
pop cx
pop dx
   jmp delay_place
   
   again_enemies:
        mov enemies_are_helpless, 0 
        
        mov enemy1_atr, 00001010b
        mov enemy2_atr, 00001101b
        mov enemy3_atr, 00001111b     
           
    delay_place:
    call delay
        check_buffer   ; check on entered symbol                  
        jnz buffer_is_not_empty
        jmp buffer_is_empty
  
    buffer_is_not_empty:
        read_from_buffer
        cmp ah, 01h            ; check on esc
        je far_exit1
        
        cmp ah, 11h	           ; check on up (W key)
        je up_is_pressed
        
        cmp ah, 20h            ; check on right (D key)
        je right_is_pressed
        
        cmp ah, 1Fh            ; check on down (S key)
        je down_is_pressed
        
        cmp ah, 1Eh	           ; check on left (A key)
        je left_is_pressed        
        jmp buffer_is_empty    ; pressed key no matters
        
        far_exit1:
            jmp far ptr far_exit

        up_is_pressed:
    mov ch, pacman_y
    dec ch
    set_cursor pacman_x, ch 
    mov ah, 8
    int 10h
    cmp al, 219        
    je buffer_is_empty
    cmp al, 48       
    je buffer_is_empty      
            mov pacman_dir,1
            jmp buffer_is_empty
                          
        right_is_pressed:
    mov ch, pacman_x
    inc ch
    set_cursor ch, pacman_y 
    mov ah, 8
    int 10h
    cmp al, 48        
    je buffer_is_empty         
            mov pacman_dir,2
            jmp buffer_is_empty   
       
        down_is_pressed:   
    mov ch, pacman_y
    inc ch
    set_cursor pacman_x, ch 
    mov ah, 8
    int 10h
    cmp al, 219        
    je buffer_is_empty
    cmp al, 48       
    je buffer_is_empty                 
        mov pacman_dir,3
        jmp buffer_is_empty        
                
        left_is_pressed:
    mov ch, pacman_x
    dec ch
    set_cursor ch, pacman_y 
    mov ah, 8
    int 10h
    cmp al, 48        
    je buffer_is_empty          
            mov pacman_dir,4
            jmp buffer_is_empty             
        
                
   buffer_is_empty:    
        call set_pacman_coords 
            
        move:
        ;;;;;;;;;;;enemies_print         
                    ;coord_x in cl, coord_y in ch, dir in bl
            cmp enemy1_is_alive, 1
                jne check_on_life_enemy02            
                    mov ah,  enemy1_x
                    mov enemy1_prev_x, ah 
                    mov ah,  enemy1_y
                    mov enemy1_prev_y, ah                       
                    mov cl, enemy1_x
                    mov ch, enemy1_y
                    mov bl, enemy1_dir
                    call set_enemy_coords
                    mov enemy1_x, cl
                    mov enemy1_y, ch
                    mov enemy1_dir, bl            
    
    check_on_life_enemy02:
    cmp enemy2_is_alive, 1
    jne check_on_life_enemy03       
    ;coord_x in cl, coord_y in ch, dir in bl
        mov ah,  enemy2_x
        mov enemy2_prev_x, ah 
        mov ah,  enemy2_y
        mov enemy2_prev_y, ah          
    mov cl, enemy2_x
    mov ch, enemy2_y
    mov bl, enemy2_dir
    call set_enemy_coords
    mov enemy2_x, cl
    mov enemy2_y, ch
    mov enemy2_dir, bl        
                
        check_on_life_enemy03:
        cmp enemy3_is_alive, 1
        jne enemies_symbol_print                    
                        ;coord_x in cl, coord_y in ch, dir in bl
                    mov ah,  enemy3_x
                    mov enemy3_prev_x, ah 
                    mov ah,  enemy3_y
                    mov enemy3_prev_y, ah                             
                        mov cl, enemy3_x
                        mov ch, enemy3_y
                        mov bl, enemy3_dir
                        call set_enemy_coords
                        mov enemy3_x, cl
                        mov enemy3_y, ch
                        mov enemy3_dir, bl        
    enemies_symbol_print:
        cmp enemy1_is_alive, 1
        jne enemy2_symbol_print    
    print_sym enemy1_prev_x, enemy1_prev_y, enemy1_prev_symbol, enemy1_prev_symbatr  
                    set_cursor enemy1_x, enemy1_y          ;remember
                        mov ah, 8
                        int 10h
                        mov enemy1_prev_symbol, al           
                        mov enemy1_prev_symbatr, ah
        enemy2_symbol_print:
        cmp enemy2_is_alive, 1
        jne enemy3_symbol_print                
    print_sym enemy2_prev_x, enemy2_prev_y, enemy2_prev_symbol, enemy2_prev_symbatr  
                    set_cursor enemy2_x, enemy2_y          ;remember
                        mov ah, 8
                        int 10h
                        mov enemy2_prev_symbol, al           
                        mov enemy2_prev_symbatr, ah
        enemy3_symbol_print:                
        cmp enemy3_is_alive, 1
        jne countinue_enemies_symbol_print                  
    print_sym enemy3_prev_x, enemy3_prev_y, enemy3_prev_symbol, enemy3_prev_symbatr  
                    set_cursor enemy3_x, enemy3_y          ;remember
                        mov ah, 8
                        int 10h
                        mov enemy3_prev_symbol, al           
                        mov enemy3_prev_symbatr, ah
    countinue_enemies_symbol_print:                                                                          
    call print_enemies    
                                         
        ;;;;;;;;;;;pacman_print
        set_cursor pacman_x, pacman_y
    mov ah, 8       ; compare with bonus
    int 10h
    cmp al, 249               
    je add_10
    cmp al, 15      ; compare with bonus2
    je make_enemies_helpless
                 
    cmp al, 1       ; compare with enemy
    jne continue_move404 
    cmp enemies_are_helpless, 0
    je far_exit4
    cmp ah, enemy1_atr 
    jne check_on_life_enemy2 
    mov enemy1_is_alive, 0
    mov enemy1_x, 0 
    mov enemy1_y, 0
    
        check_on_life_enemy2:
    cmp ah, enemy2_atr 
    jne check_on_life_enemy3 
    mov enemy2_is_alive, 0    
    mov enemy2_x, 0 
    mov enemy2_y, 0
                       
        check_on_life_enemy3:               
    cmp ah, enemy3_atr 
    jne end_check_on_life_enemies 
    mov enemy3_is_alive, 0     
    mov enemy3_x, 0 
    mov enemy3_y, 0
    
        end_check_on_life_enemies:    
    jmp continue_move
    continue_move404:
        jmp far ptr continue_move  
    
    far_exit4:
        jmp far ptr far_exit
       
    add_10:    
        add score_num, 10
            set_cursor 47, 24 
            push ax
            mov ax, score_num 
            call PRINT_NUM_UNS
            pop ax 
        jmp continue_move
        
    make_enemies_helpless:
        mov enemies_are_helpless, 1
        
        mov enemy1_atr, 10001010b
        mov enemy2_atr, 10001101b
        mov enemy3_atr, 10001111b
        
        get_timer_value 
        add dx, enemies_are_helpless_fulltime  
        mov enemies_are_helpless_endtime, dx
     
        jmp continue_move

   
        continue_move:
        call print_pacman 
        cmp pacman_dir, 0
            jne print_space
            print_sym pacman_prev_x, pacman_prev_y, pacman_dir_symb, 14 
            jmp main_game_loop
    print_space:    
        print_sym pacman_prev_x, pacman_prev_y, 32, 0      
        jmp main_game_loop 

   far_exit:
        jmp far ptr exit  
        ret
    game_process endp
  
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    SET_PACMAN_COORDS proc 
        mov ah, pacman_x
        mov pacman_prev_x, ah                
        mov ah, pacman_y
        mov pacman_prev_y, ah   
            
        cmp pacman_dir, 1
        je up_direction                            
        cmp pacman_dir, 2
        je right_direction
        cmp pacman_dir, 3
        je down_direction
        cmp pacman_dir, 4
        je left_direction1
        
    stop_dir:        
        mov pacman_dir, 0
        jmp move
        
    left_direction1:
        jmp far ptr left_direction                        
        
    up_direction:
            mov ch, pacman_y
            dec ch
            
            mov pacman_dir_symb, 194 ;;;;;
            
            set_cursor pacman_x, ch
            mov ah, 8
            int 10h
            cmp al, 219 
            je stop_dir
            cmp ch, 0
            je stop_dir          
         dec pacman_y
         jmp move
                 
    right_direction:
            mov ch, pacman_x
            inc ch
            
            mov pacman_dir_symb, 2 ;;;;;
            
            set_cursor ch, pacman_y
            mov ah, 8
            int 10h
            cmp al, 48 
            je stop_dir        
        inc pacman_x
        cmp pacman_x, 80
        jne move1             
            mov pacman_x, 0
        jmp move
        
        move1:
            jmp far ptr move 
        
    down_direction:
            mov ch, pacman_y
            inc ch       
            
            mov pacman_dir_symb, 193 ;;;;;
            
            set_cursor pacman_x, ch
            mov ah, 8
            int 10h
            cmp al, 219 
            je stop_dir1        
        inc pacman_y
        jmp move
        
        stop_dir1:
         jmp far ptr stop_dir
        
    left_direction:
            mov ch, pacman_x
            dec ch  
            
            mov pacman_dir_symb, 192 ;;;;;
            
            set_cursor ch, pacman_y
            mov ah, 8
            int 10h
            cmp al, 48 
            je stop_direction    
        dec pacman_x
            cmp pacman_x, -1
            jne move2             
            mov pacman_x, 79             
        jmp move 
     
     move2:
        jmp far ptr move           
     stop_direction:
         mov pacman_dir,0   
         jmp move         
        ret
    set_pacman_coords endp 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;    
    DELAY proc                   
    	push ax                     
    	push bx                     
    	push cx                     
    	push dx                     
                                
    	get_timer_value         
                             
    	add dx, delay_time      
    	mov bx, dx              
                                 
    checkTimeLoop:                   
    	get_timer_value                
    	cmp dx, bx			         
    	jl checkTimeLoop              
                                  
    	pop dx                       
    	pop cx                       
    	pop bx                       
    	pop ax                       
    	ret                          
    delay endp                         
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; this procedure prints out an unsigned
; number in AX (not just a single digit)
; allowed values are from 0 to 65535 (FFFF)
PRINT_NUM_UNS   PROC    NEAR
        PUSH    AX
        PUSH    BX
        PUSH    CX
        PUSH    DX

        ; flag to prevent printing zeros before number:
        MOV     CX, 1

        ; (result of "/ 10000" is always less or equal to 9).
        MOV     BX, 10000       ; 2710h - divider.

        ; AX is zero?
        CMP     AX, 0
        JZ      print_zero

begin_print:
        ; check divider (if zero go to end_print):
        CMP     BX,0
        JZ      end_print
        ; avoid printing zeros before number:
        CMP     CX, 0
        JE      calc
        ; if AX<BX then result of DIV will be zero:
        CMP     AX, BX
        JB      skip
calc:
        MOV     CX, 0   ; set flag.

        MOV     DX, 0
        DIV     BX      ; AX = DX:AX / BX   (DX=remainder).
        ; print last digit
        ; AH is always ZERO, so it's ignored
        ADD     AL, 30h    ; convert to ASCII code.
        PUTC    AL
        MOV     AX, DX  ; get remainder from last div.
skip:
        ; calculate BX=BX/10
        PUSH    AX
        MOV     DX, 0
        MOV     AX, BX
        DIV     CS:ten  ; AX = DX:AX / 10   (DX=remainder).
        MOV     BX, AX
        POP     AX
        JMP     begin_print
        
print_zero:
        PUTC    '0'        
end_print:

        POP     DX
        POP     CX
        POP     BX
        POP     AX
        RET
PRINT_NUM_UNS   ENDP
ten             DW      10

YOU_WON proc
        xor     si, si     
        mov     bh, 0   ; num of page
        mov     cx, 1   ; num of symbols repeat      
        mov     dl, 7   ; current column
        mov     dh, 7   ; current row
            mov     si, offset winBoard
            jmp     nnext_char
            
    sstop_print1:
        jmp far ptr sstop_print
        
        nnext_row:       ; new line
        inc     dh
        cmp     dh, 14
        je      sstop_print1
        mov     dl, 7           
    
        nnext_char:        
        ;cmp [si], elem               
        mov     ax, [si]
        cmp     al, ' '          
        je is_sss
        cmp     al, 'v'
        je isV
                cmp     al, '1'
        je is0001        
                cmp     al, '0'
        je is000
                cmp     al, '9'
        je is009
                cmp     al, '2'
        je is0002        
                cmp     al, '3'
        je is0003 
                cmp     al, '4'
        je is0004 

        is0001:
        jmp far ptr is001        
        is0002:
        jmp far ptr is002
        is0003:
        jmp far ptr is003
        is0004:
        jmp far ptr is004        
               
        is_sss:  
              print_sym dl, dh, 219, 0
              jmp nnew_elem_print
             
        isV:
              print_sym dl, dh, 176, 00001010b
              jmp nnew_elem_print
        is000:      
             print_sym dl, dh, 205, 00001111b
             jmp nnew_elem_print 
        is009:
             print_sym dl, dh, 186, 00001111b
             jmp nnew_elem_print
        is001:
             print_sym dl, dh, 201, 00001111b
             jmp nnew_elem_print
        is002:
             print_sym dl, dh, 187, 00001111b
             jmp nnew_elem_print
        is003:
             print_sym dl, dh, 200, 00001111b
             jmp nnew_elem_print
        is004:     
             print_sym dl, dh, 188, 00001111b 

        ; next elem
        nnew_elem_print:
        inc     si
        inc     dl
        cmp     dl, 72   
        je      nnext_row1
        jmp     nnext_char  
        
        nnext_row1:
            jmp far ptr nnext_row
          
    sstop_print:
xor ax, ax    
mov ah, 0
int 16h
    
    ret
you_won endp
   
end	start                     
