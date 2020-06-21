; This program serves as an example for coding the writer.cpp example

.386                   ; For 80386 CPUs or higher.
.model flat, stdcall   ; Windows is always the 32-bit FLAT model

option casemap:none    ; Masm32 will use case-sensitive labels.

.data

MyBoxText   db  "Hello World!",0

.code

MyMsgBox:

    MessageBoxA PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD
    invoke MessageBoxA, 0, ADDR MyBoxText, ADDR MyBoxText, 0
    
    ExitProcess PROTO STDCALL :DWORD
    invoke ExitProcess,0

    hlt

end MyMsgBox
