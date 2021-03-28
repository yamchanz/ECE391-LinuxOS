#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}
/* divide_error()
    DESCRIPTION: attempts to divide 2 by 0, handled by interrupt 0x00
    INPUTS: none
    OUTPUTS: PASS if successful, interrupt handler if not
    RETURN VALUE: none
    SIDE EFFECTS: freezes screen with handler message
*/
int divide_error() {
	TEST_HEADER;
	int num = 2;
	int denom = 0;
	num /= denom;
	denom = num;

	return PASS;
}
/* opcode_error
    DESCRIPTION: attempts to move from a non-existent register
    INPUTS: none
    OUTPUTS: PASS if successful, interrupt handler if not
    RETURN VALUE: none
    SIDE EFFECTS: freezes screen with handler message
*/
int opcode_error() {
	TEST_HEADER;
	asm volatile ("movl %cr6, %eax");

	return PASS;
}
/* opcode_error
    DESCRIPTION: calls the system call handler at index 0x80 of IDT
    INPUTS: none
    OUTPUTS: PASS if successful, interrupt handler if not
    RETURN VALUE: none
    SIDE EFFECTS: freezes screen with handler message
*/
int sys_call_test() {
	TEST_HEADER;
	asm volatile ("int $0x80");

	return PASS;
}


// add more tests here

int keyboard_test() {
	TEST_HEADER;
	return PASS;
}

/* video_mem_paging_test - CP1
 * DESCRIPTION: dereferences the video memory portion of virtual memory.
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
int video_mem_paging_test() {
	TEST_HEADER;

	int* ptr = (int*) 0x000B8000; // test at video memory address
	int deref;
	deref = *ptr;
	return PASS;
}

/* kernel_paging_test - CP1
 * DESCRIPTION: dereferences the kernel memory portion of virtual memory.
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
int kernel_paging_test() {
	TEST_HEADER;

	int* ptr = (int*) 0x00400000; // test at kernel address
	int deref;
	deref = *ptr;
	return PASS;
}

/* not_present_paging_test - CP1
 * DESCRIPTION: dereferences the memory portion not present in virtual memory.
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Should cause a page fault exception to be printed to screen.
 */
int not_present_paging_test() {
	TEST_HEADER;

	int* ptr = (int*) 0x1000000; // test at 16MB - nothing should be there
	int deref;
	deref = *ptr; // should cause exception
	return FAIL;
}

/* Checkpoint 2 tests */

/* rtc_freq_test - CP2
 * DESCRIPTION: cycles the RTC through all sets of frequences <= 1024 by writing into Register A, starting from 2
 * INPUTS: none
 * OUTPUTS: prints 1 for each interrupt
 * RETURN VALUE: none
 * SIDE EFFECTS: changes frequency on the RTC.
 */
int rtc_freq_test() {
	TEST_HEADER;

	// initialize rtc and set frequency to 2
	int rtc = rtc_open(0);
	// temp value to store result of rtc command
	int rtc_cmd;

	uint32_t freq[1];
	// set start frequency to 2
	*freq = 2;
	while(*freq <= HIGH_LIMIT_FREQ) {
		clear();
		// error checking for NULL case or non-log2 number
		if(rtc_write(rtc, freq, 4) != 0) {
			return FAIL;
		}
		int read_int;
		for(read_int = 0; read_int < NUM_READ_INTS; read_int++) {
			printf("%d", *freq);
			// wait for interrupt
			rtc_cmd = rtc_read(rtc, 0, 0);
		}

		*freq *= 2;
	}
	// close RTC
	rtc_cmd = rtc_close(rtc);
	
	return PASS;

}
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("not_present_paging_test", not_present_paging_test());
	// TEST_OUTPUT("kernel_paging_test", kernel_paging_test());
	// TEST_OUTPUT("video_mem_paging_test", video_mem_paging_test());

	// launch your tests here
	// divide_error();
	// opcode_error();
	// sys_call_test();

	rtc_freq_test();


}
