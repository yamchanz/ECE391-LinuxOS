#include "x86_desc.h"
#include "int.h"
#include "linkage.h"

void (*exceptions[20])() = {
    Divide_Error_Exception, Debug_Exception, NMI_Interrupt, Breakpoint_Exception, Overflow_Exception, BOUND_Range_Exceeded_Exception, Invalid_Opcode_Exception, 
    Device_Not_Available_Exception, Double_Fault_Exception, Coprocessor_Segment_Overrun, Invalid_TSS_Exception, Segment_Not_Present, Stack Fault Exception,
    General_Protection_Exception, Page_Fault_Exception, reserved, x87_FPU_Floating_Point_Error, Alignment_Check_Exception, Machine_Check_Exception, 
    SIMD_Floating_Point_Exception
};

void (*interrupts[3])() = {
    timer_chip, keyboard, real_time_clock
};

void set_handler(){
    int i,j;
    for(i=0;i<32; i ++) {
        idt[i].seg_selector = KERNEL_CS;
        idt[i].present = 1;
        idt[i].size = 1;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 0;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved0 = 0;
        idt[i].dpl = 0;
        if(i == 15 || (i>19 || i<32)){
            SET_IDT_ENTRY(idt[i],reserved);
        }else {
            SET_IDT_ENTRY(idt[i],exceptions[i]);
        }

    }

    for(j=0;j<3){
        if( j == 0 || j == 1){
            idt[i+0x20].seg_selector = KERNEL_CS;
            idt[i+0x20].present = 1;
            idt[i+0x20].size = 1;
            idt[i+0x20].reserved4 = 0;
            idt[i+0x20].reserved3 = 0;
            idt[i+0x20].reserved2 = 1;
            idt[i+0x20].reserved1 = 1;
            idt[i+0x20].reserved0 = 0;
            idt[i+0x20].dpl = 0;
            SET_IDT_ENTRY(idt[i+0x20],interrupts[i]);
        } else {
            idt[i+0x20].seg_selector = KERNEL_CS;
            idt[i+0x20].present = 1;
            idt[i+0x20].size = 1;
            idt[i+0x20].reserved4 = 0;
            idt[i+0x20].reserved3 = 0;
            idt[i+0x20].reserved2 = 1;
            idt[i+0x20].reserved1 = 1;
            idt[i+0x20].reserved0 = 0;
            idt[i+0x20].dpl = 0;
            SET_IDT_ENTRY(idt[j+0x25],interrupts[j]);
        }
    }
    idt[0x80].seg_selector = KERNEL_CS;
    idt[0x80].present = 1;
    idt[0x80].size = 1;
    idt[0x80].reserved4 = 0;
    idt[0x80].reserved3 = 0;
    idt[0x80].reserved2 = 1;
    idt[0x80].reserved1 = 1;
    idt[0x80].reserved0 = 0;
    idt[0x80].dpl = 3;
            SET_IDT_ENTRY(idt[0x80],system_call_handler);

    return;
}


void system_call_handler(){
    printf("system calls");
    
}

void timer_chip() {

}

void keyboard() {

}

void real_time_clock() {

}

void reserved() {
    printf("reserved interrupt");
    while(1){

    }
}
void Divide_Error_Exception (){
    
    printf("Divide Error Exception (#DE)");
    while(1) {

    }
}

void Debug_Exception() {
    printf("Debug Exception (#DB)");
    while(1) {

    }
}

void NMI_Interrupt() {
    printf("Debug Exception (#DB)");
    while(1) {

    }
}

void Breakpoint_Exception() {
    printf("Breakpoint Exception (#BP)");
    while(1) {

    }
}

void Overflow_Exception() {
    printf("Overflow Exception (#OF)");
    while(1) {

    }
}

void BOUND_Range_Exceeded_Exception() {
    printf("BOUND Range Exceeded Exception (#BR)");
    while(1) {

    }
}

void Invalid_Opcode_Exception() {
    printf("Invalid Opcode Exception (#UD)");
    while(1) {

    }
}

void Device_Not_Available_Exception() {
    printf("Device Not Available Exception (#NM)");
    while(1){

    }
}

void Double_Fault_Exception() {
    printf("Double Fault Exception (#DF)");
    while(1){

    }
}

void Coprocessor_Segment_Overrun() {
    printf("Coprocessor_Segment_Overrun");
    while(1){

    }
}

void Invalid_TSS_Exception () {
    printf("Invalid_TSS_Exception");
    while(1){

    }
}

void Segment_Not_Present () {
    printf("Segment Not Present (#NP)");
    while(1){

    }
}

void Stack_Fault_Exception() {
    printf("Stack Fault Exception (#SS)");
    while(1){

    }
}

void General_Protection_Exception() {
    printf("General Protection Exception (#GP)");
    while(1){

    }
}

void Page_Fault_Exception() {
    printf("Page-Fault_Exception (#PF)");
    while(1){

    }
}

void x87_FPU_Floating_Point_Error () {
     printf("x87 FPU Floating-Point Error (#MF)");
    while(1){

    }
}

void Alignment_Check_Exception() {
    printf("Alignment Check Exception (#AC)");
    while(1){

    }
}

void Machine_Check_Exception() {
    printf("Machine-Check Exception (#MC)");
    while(1){

    }
}

void SIMD_Floating_Point_Exception () {
    printf("SIMD Floating-Point Exception (#XF)");
    while(1){

    }
}