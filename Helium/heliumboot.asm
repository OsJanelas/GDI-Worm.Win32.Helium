[BITS 16]
[ORG 0x7C00]

start:
    ; Configura o ambiente
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00      ; Coloca a pilha antes do código

    ; Ativa Modo 13h (320x200, 256 cores)
    mov ax, 0x0013
    int 0x10

    ; 1. Imprime o texto usando a BIOS
    call print_msg

    ; 2. Configura ES para a memória de vídeo (0xA000)
    mov ax, 0xA000
    mov es, ax

render_fractal:
    xor di, di          ; Ponteiro de pixel (0 a 63999)

.loop_y:
    ; Calcula X e Y a partir de DI
    mov ax, di
    xor dx, dx
    mov bx, 320
    div bx              ; AX = Y, DX = X

    ; Pula as primeiras linhas para não apagar o texto
    cmp ax, 16
    jl .next_pixel

    ; --- Lógica Sierpinski ---
    mov bx, ax          ; BX = Y
    and bx, dx          ; Operação bitwise: (Y AND X)
    
    jnz .draw_black     ; Se (Y AND X) != 0, pinta de preto (ou pula)
    
    ; Se for 0, desenha com cor RGB baseada em X e Y
    mov al, dl          ; Cor baseada em X
    add al, byte [color_offset] ; Adiciona "movimento" à cor
    mov [es:di], al
    jmp .next_pixel

.draw_black:
    mov byte [es:di], 0

.next_pixel:
    inc di
    cmp di, 64000
    jne .loop_y

    ; Incrementa o offset da cor para um efeito psicodélico
    inc byte [color_offset]
    
    ; Loop infinito (ou jmp render_fractal para animar)
    jmp render_fractal

; --- Sub-rotina de Texto ---
print_msg:
    mov si, msg
    mov bl, 32          ; Cor inicial (paleta VGA)
.loop:
    lodsb               ; Carrega caractere de [SI] em AL
    test al, al         ; É o fim da string?
    jz .done
    
    mov ah, 0x0E        ; BIOS Teletype
    int 0x10            ; O registrador BL controla a cor em alguns modos
    
    inc bl              ; Próxima cor
    cmp bl, 50          ; Ciclo de cores
    jne .loop
    mov bl, 32
    jmp .loop
.done:
    ret

; --- Dados ---
msg: db "So... U can't boot your system", 0
color_offset: db 0

; Assinatura de Boot
times 510-($-$$) db 0
dw 0xAA55