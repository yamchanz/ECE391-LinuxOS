#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

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

int divide_error() {
	TEST_HEADER;
	int num = 2;
	int denom = 0;

	num /= denom;

	denom = num;

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
/* terminal_string_test - CP1
 * DESCRIPTION: test the terminal to print max 128 characters
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
int terminal_string_test() {
	TEST_HEADER;
	int32_t i;
	uint8_t test1[128], test2[200];

	for (i = 0; i < 127; ++i) {
		test1[i] = 'a';
		test2[i] = 'a';
	}
	test1[127] = 'b';
	for (i = 127; i < 200; ++i) {
		test2[i] = 'b';
	}
	write_terminal(test1);
	write_terminal(test2);
	return PASS;
}
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("not_present_paging_test", not_present_paging_test());
	//TEST_OUTPUT("kernel_paging_test", kernel_paging_test());
	//TEST_OUTPUT("video_mem_paging_test", video_mem_paging_test());

	// launch your tests here
	// divide_error();
	terminal_string_test();
}
