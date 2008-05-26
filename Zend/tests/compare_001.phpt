--TEST--
comparing different variables for equality
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
--FILE--
<?php

class test {
}

$a = array(
	array(1,2,3),
	"",
	1,
	2.5,
	0,
	"string",
	"123",
	"2.5",
	NULL,
	true,
	false,
	new stdclass,
	new stdclass,
	new test,
	array(),
	-PHP_INT_MAX-1,
	(string)(-PHP_INT_MAX-1),
);

$var_cnt = count($a);

function my_dump($var) {
	ob_start();
	var_dump($var);
	$buf = ob_get_clean();
	echo str_replace("\n", "", $buf);
}

foreach($a as $var) {
	for ($i = 0; $i < $var_cnt; $i++) {
		my_dump($var);
		echo ($var == $a[$i]) ? " == " : " != ";
		my_dump($a[$i]);
		echo "\n";
	}
}	

echo "Done\n";
?>
--EXPECTF--
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} == array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != unicode(0) ""
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != int(1)
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != float(2.5)
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != int(0)
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != unicode(6) "string"
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != unicode(3) "123"
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != unicode(3) "2.5"
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != NULL
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} == bool(true)
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != bool(false)
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != object(stdClass)#1 (0) {}
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != object(stdClass)#2 (0) {}
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != object(test)#3 (0) {}
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != array(0) {}
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != int(-2147483648)
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} != unicode(11) "-2147483648"
unicode(0) "" != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
unicode(0) "" == unicode(0) ""
unicode(0) "" != int(1)
unicode(0) "" != float(2.5)
unicode(0) "" == int(0)
unicode(0) "" != unicode(6) "string"
unicode(0) "" != unicode(3) "123"
unicode(0) "" != unicode(3) "2.5"
unicode(0) "" == NULL
unicode(0) "" != bool(true)
unicode(0) "" == bool(false)
unicode(0) "" != object(stdClass)#1 (0) {}
unicode(0) "" != object(stdClass)#2 (0) {}
unicode(0) "" != object(test)#3 (0) {}
unicode(0) "" != array(0) {}
unicode(0) "" != int(-2147483648)
unicode(0) "" != unicode(11) "-2147483648"
int(1) != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
int(1) != unicode(0) ""
int(1) == int(1)
int(1) != float(2.5)
int(1) != int(0)
int(1) != unicode(6) "string"
int(1) != unicode(3) "123"
int(1) != unicode(3) "2.5"
int(1) != NULL
int(1) == bool(true)
int(1) != bool(false)
int(1)
Notice: Object of class stdClass could not be converted to int in %s on line %d
 == object(stdClass)#1 (0) {}
int(1)
Notice: Object of class stdClass could not be converted to int in %s on line %d
 == object(stdClass)#2 (0) {}
int(1)
Notice: Object of class test could not be converted to int in %s on line %d
 == object(test)#3 (0) {}
int(1) != array(0) {}
int(1) != int(-2147483648)
int(1) != unicode(11) "-2147483648"
float(2.5) != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
float(2.5) != unicode(0) ""
float(2.5) != int(1)
float(2.5) == float(2.5)
float(2.5) != int(0)
float(2.5) != unicode(6) "string"
float(2.5) != unicode(3) "123"
float(2.5) == unicode(3) "2.5"
float(2.5) != NULL
float(2.5) == bool(true)
float(2.5) != bool(false)
float(2.5)
Notice: Object of class stdClass could not be converted to double in %s on line %d
 != object(stdClass)#1 (0) {}
float(2.5)
Notice: Object of class stdClass could not be converted to double in %s on line %d
 != object(stdClass)#2 (0) {}
float(2.5)
Notice: Object of class test could not be converted to double in %s on line %d
 != object(test)#3 (0) {}
float(2.5) != array(0) {}
float(2.5) != int(-2147483648)
float(2.5) != unicode(11) "-2147483648"
int(0) != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
int(0) == unicode(0) ""
int(0) != int(1)
int(0) != float(2.5)
int(0) == int(0)
int(0) == unicode(6) "string"
int(0) != unicode(3) "123"
int(0) != unicode(3) "2.5"
int(0) == NULL
int(0) != bool(true)
int(0) == bool(false)
int(0)
Notice: Object of class stdClass could not be converted to int in %s on line %d
 != object(stdClass)#1 (0) {}
int(0)
Notice: Object of class stdClass could not be converted to int in %s on line %d
 != object(stdClass)#2 (0) {}
int(0)
Notice: Object of class test could not be converted to int in %s on line %d
 != object(test)#3 (0) {}
int(0) != array(0) {}
int(0) != int(-2147483648)
int(0) != unicode(11) "-2147483648"
unicode(6) "string" != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
unicode(6) "string" != unicode(0) ""
unicode(6) "string" != int(1)
unicode(6) "string" != float(2.5)
unicode(6) "string" == int(0)
unicode(6) "string" == unicode(6) "string"
unicode(6) "string" != unicode(3) "123"
unicode(6) "string" != unicode(3) "2.5"
unicode(6) "string" != NULL
unicode(6) "string" == bool(true)
unicode(6) "string" != bool(false)
unicode(6) "string" != object(stdClass)#1 (0) {}
unicode(6) "string" != object(stdClass)#2 (0) {}
unicode(6) "string" != object(test)#3 (0) {}
unicode(6) "string" != array(0) {}
unicode(6) "string" != int(-2147483648)
unicode(6) "string" != unicode(11) "-2147483648"
unicode(3) "123" != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
unicode(3) "123" != unicode(0) ""
unicode(3) "123" != int(1)
unicode(3) "123" != float(2.5)
unicode(3) "123" != int(0)
unicode(3) "123" != unicode(6) "string"
unicode(3) "123" == unicode(3) "123"
unicode(3) "123" != unicode(3) "2.5"
unicode(3) "123" != NULL
unicode(3) "123" == bool(true)
unicode(3) "123" != bool(false)
unicode(3) "123" != object(stdClass)#1 (0) {}
unicode(3) "123" != object(stdClass)#2 (0) {}
unicode(3) "123" != object(test)#3 (0) {}
unicode(3) "123" != array(0) {}
unicode(3) "123" != int(-2147483648)
unicode(3) "123" != unicode(11) "-2147483648"
unicode(3) "2.5" != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
unicode(3) "2.5" != unicode(0) ""
unicode(3) "2.5" != int(1)
unicode(3) "2.5" == float(2.5)
unicode(3) "2.5" != int(0)
unicode(3) "2.5" != unicode(6) "string"
unicode(3) "2.5" != unicode(3) "123"
unicode(3) "2.5" == unicode(3) "2.5"
unicode(3) "2.5" != NULL
unicode(3) "2.5" == bool(true)
unicode(3) "2.5" != bool(false)
unicode(3) "2.5" != object(stdClass)#1 (0) {}
unicode(3) "2.5" != object(stdClass)#2 (0) {}
unicode(3) "2.5" != object(test)#3 (0) {}
unicode(3) "2.5" != array(0) {}
unicode(3) "2.5" != int(-2147483648)
unicode(3) "2.5" != unicode(11) "-2147483648"
NULL != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
NULL == unicode(0) ""
NULL != int(1)
NULL != float(2.5)
NULL == int(0)
NULL != unicode(6) "string"
NULL != unicode(3) "123"
NULL != unicode(3) "2.5"
NULL == NULL
NULL != bool(true)
NULL == bool(false)
NULL != object(stdClass)#1 (0) {}
NULL != object(stdClass)#2 (0) {}
NULL != object(test)#3 (0) {}
NULL == array(0) {}
NULL != int(-2147483648)
NULL != unicode(11) "-2147483648"
bool(true) == array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
bool(true) != unicode(0) ""
bool(true) == int(1)
bool(true) == float(2.5)
bool(true) != int(0)
bool(true) == unicode(6) "string"
bool(true) == unicode(3) "123"
bool(true) == unicode(3) "2.5"
bool(true) != NULL
bool(true) == bool(true)
bool(true) != bool(false)
bool(true) == object(stdClass)#1 (0) {}
bool(true) == object(stdClass)#2 (0) {}
bool(true) == object(test)#3 (0) {}
bool(true) != array(0) {}
bool(true) == int(-2147483648)
bool(true) == unicode(11) "-2147483648"
bool(false) != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
bool(false) == unicode(0) ""
bool(false) != int(1)
bool(false) != float(2.5)
bool(false) == int(0)
bool(false) != unicode(6) "string"
bool(false) != unicode(3) "123"
bool(false) != unicode(3) "2.5"
bool(false) == NULL
bool(false) != bool(true)
bool(false) == bool(false)
bool(false) != object(stdClass)#1 (0) {}
bool(false) != object(stdClass)#2 (0) {}
bool(false) != object(test)#3 (0) {}
bool(false) == array(0) {}
bool(false) != int(-2147483648)
bool(false) != unicode(11) "-2147483648"
object(stdClass)#1 (0) {} != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
object(stdClass)#1 (0) {} != unicode(0) ""
object(stdClass)#1 (0) {}
Notice: Object of class stdClass could not be converted to int in %s on line %d
 == int(1)
object(stdClass)#1 (0) {}
Notice: Object of class stdClass could not be converted to double in %s on line %d
 != float(2.5)
object(stdClass)#1 (0) {}
Notice: Object of class stdClass could not be converted to int in %s on line %d
 != int(0)
object(stdClass)#1 (0) {} != unicode(6) "string"
object(stdClass)#1 (0) {} != unicode(3) "123"
object(stdClass)#1 (0) {} != unicode(3) "2.5"
object(stdClass)#1 (0) {} != NULL
object(stdClass)#1 (0) {} == bool(true)
object(stdClass)#1 (0) {} != bool(false)
object(stdClass)#1 (0) {} == object(stdClass)#1 (0) {}
object(stdClass)#1 (0) {} == object(stdClass)#2 (0) {}
object(stdClass)#1 (0) {} != object(test)#3 (0) {}
object(stdClass)#1 (0) {} != array(0) {}
object(stdClass)#1 (0) {}
Notice: Object of class stdClass could not be converted to int in %s on line %d
 != int(-2147483648)
object(stdClass)#1 (0) {} != unicode(11) "-2147483648"
object(stdClass)#2 (0) {} != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
object(stdClass)#2 (0) {} != unicode(0) ""
object(stdClass)#2 (0) {}
Notice: Object of class stdClass could not be converted to int in %s on line %d
 == int(1)
object(stdClass)#2 (0) {}
Notice: Object of class stdClass could not be converted to double in %s on line %d
 != float(2.5)
object(stdClass)#2 (0) {}
Notice: Object of class stdClass could not be converted to int in %s on line %d
 != int(0)
object(stdClass)#2 (0) {} != unicode(6) "string"
object(stdClass)#2 (0) {} != unicode(3) "123"
object(stdClass)#2 (0) {} != unicode(3) "2.5"
object(stdClass)#2 (0) {} != NULL
object(stdClass)#2 (0) {} == bool(true)
object(stdClass)#2 (0) {} != bool(false)
object(stdClass)#2 (0) {} == object(stdClass)#1 (0) {}
object(stdClass)#2 (0) {} == object(stdClass)#2 (0) {}
object(stdClass)#2 (0) {} != object(test)#3 (0) {}
object(stdClass)#2 (0) {} != array(0) {}
object(stdClass)#2 (0) {}
Notice: Object of class stdClass could not be converted to int in %s on line %d
 != int(-2147483648)
object(stdClass)#2 (0) {} != unicode(11) "-2147483648"
object(test)#3 (0) {} != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
object(test)#3 (0) {} != unicode(0) ""
object(test)#3 (0) {}
Notice: Object of class test could not be converted to int in %s on line %d
 == int(1)
object(test)#3 (0) {}
Notice: Object of class test could not be converted to double in %s on line %d
 != float(2.5)
object(test)#3 (0) {}
Notice: Object of class test could not be converted to int in %s on line %d
 != int(0)
object(test)#3 (0) {} != unicode(6) "string"
object(test)#3 (0) {} != unicode(3) "123"
object(test)#3 (0) {} != unicode(3) "2.5"
object(test)#3 (0) {} != NULL
object(test)#3 (0) {} == bool(true)
object(test)#3 (0) {} != bool(false)
object(test)#3 (0) {} != object(stdClass)#1 (0) {}
object(test)#3 (0) {} != object(stdClass)#2 (0) {}
object(test)#3 (0) {} == object(test)#3 (0) {}
object(test)#3 (0) {} != array(0) {}
object(test)#3 (0) {}
Notice: Object of class test could not be converted to int in %s on line %d
 != int(-2147483648)
object(test)#3 (0) {} != unicode(11) "-2147483648"
array(0) {} != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
array(0) {} != unicode(0) ""
array(0) {} != int(1)
array(0) {} != float(2.5)
array(0) {} != int(0)
array(0) {} != unicode(6) "string"
array(0) {} != unicode(3) "123"
array(0) {} != unicode(3) "2.5"
array(0) {} == NULL
array(0) {} != bool(true)
array(0) {} == bool(false)
array(0) {} != object(stdClass)#1 (0) {}
array(0) {} != object(stdClass)#2 (0) {}
array(0) {} != object(test)#3 (0) {}
array(0) {} == array(0) {}
array(0) {} != int(-2147483648)
array(0) {} != unicode(11) "-2147483648"
int(-2147483648) != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
int(-2147483648) != unicode(0) ""
int(-2147483648) != int(1)
int(-2147483648) != float(2.5)
int(-2147483648) != int(0)
int(-2147483648) != unicode(6) "string"
int(-2147483648) != unicode(3) "123"
int(-2147483648) != unicode(3) "2.5"
int(-2147483648) != NULL
int(-2147483648) == bool(true)
int(-2147483648) != bool(false)
int(-2147483648)
Notice: Object of class stdClass could not be converted to int in %s on line %d
 != object(stdClass)#1 (0) {}
int(-2147483648)
Notice: Object of class stdClass could not be converted to int in %s on line %d
 != object(stdClass)#2 (0) {}
int(-2147483648)
Notice: Object of class test could not be converted to int in %s on line %d
 != object(test)#3 (0) {}
int(-2147483648) != array(0) {}
int(-2147483648) == int(-2147483648)
int(-2147483648) == unicode(11) "-2147483648"
unicode(11) "-2147483648" != array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
unicode(11) "-2147483648" != unicode(0) ""
unicode(11) "-2147483648" != int(1)
unicode(11) "-2147483648" != float(2.5)
unicode(11) "-2147483648" != int(0)
unicode(11) "-2147483648" != unicode(6) "string"
unicode(11) "-2147483648" != unicode(3) "123"
unicode(11) "-2147483648" != unicode(3) "2.5"
unicode(11) "-2147483648" != NULL
unicode(11) "-2147483648" == bool(true)
unicode(11) "-2147483648" != bool(false)
unicode(11) "-2147483648" != object(stdClass)#1 (0) {}
unicode(11) "-2147483648" != object(stdClass)#2 (0) {}
unicode(11) "-2147483648" != object(test)#3 (0) {}
unicode(11) "-2147483648" != array(0) {}
unicode(11) "-2147483648" == int(-2147483648)
unicode(11) "-2147483648" == unicode(11) "-2147483648"
Done
