;Projekt JA - Algorytm Floyda-Steinberga
;Konrad von Broen
.data
.code
;
; RCX - gray
; RDX - bw_threshold
; RAX -zwrocona wartosc int
TransformPixelAsm proc
    CMP RCX, RDX    ; porownanie wartosci 2 argumentow
    ja  Return_0    ;skok jesli wartosc jest wieksza
    mov rax, 1      ;zapisanie wartosci 1 do zwracanego argumentu
    jmp end_lab     ;bezwarunkowy skok do etykiety konczacej procedury
Return_0:           ;etykieta sluzaca do zwrocenia wartosci 0
    mov rax, 0      ;zapisanie wartosci 0 do rejestru sluzacego do zwracania wartosci
end_lab:
    ret
TransformPixelAsm endp
end