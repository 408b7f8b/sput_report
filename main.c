//
// Created by root on 11.10.18.
//

#include "sput.h"

int summieren(int a, int b){

	return a + b;

}

static void testfunktion1(){

	int c;
	c = summieren(1, 1);

	sput_fail_unless(c == 2, "test 1");

}

static void testfunktion2(){

	int c;
	c = summieren(1, 1);

	sput_fail_if(c == 2, "test 2");

}

int main(){

	sput_start_testing();
	sput_enter_suite("suite1");
	sput_run_test(testfunktion1, "testklasse1");
	sput_finish_testing(); //sput_leave_suite() enthalten

	sput_enter_suite("suite2");
	sput_run_test(testfunktion2, "testklasse2");
	sput_finish_testing();

	sput_xml_schreiben_f("report1.xml");

	return sput_get_return_value();

}

