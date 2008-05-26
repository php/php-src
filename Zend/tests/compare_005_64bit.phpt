--TEST--
comparing different variables (greater or equal than)
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
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
		echo ($var >= $a[$i]) ? " >= " : " < ";
		my_dump($a[$i]);
		echo "\n";
	}
}	

echo "Done\n";
?>
--EXPECTF--
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= unicode(0) ""
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= int(1)
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= float(2.5)
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= int(0)
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= unicode(6) "string"
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= unicode(3) "123"
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= unicode(3) "2.5"
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= NULL
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= bool(true)
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= bool(false)
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} < object(stdClass)#%d (0) {}
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} < object(stdClass)#%d (0) {}
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} < object(test)#%d (0) {}
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= array(0) {}
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= int(-9223372036854775808)
array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)} >= unicode(20) "-9223372036854775808"
unicode(0) "" < array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
unicode(0) "" >= unicode(0) ""
unicode(0) "" < int(1)
unicode(0) "" < float(2.5)
unicode(0) "" >= int(0)
unicode(0) "" < unicode(6) "string"
unicode(0) "" < unicode(3) "123"
unicode(0) "" < unicode(3) "2.5"
unicode(0) "" >= NULL
unicode(0) "" < bool(true)
unicode(0) "" >= bool(false)
unicode(0) "" < object(stdClass)#%d (0) {}
unicode(0) "" < object(stdClass)#%d (0) {}
unicode(0) "" < object(test)#%d (0) {}
unicode(0) "" < array(0) {}
unicode(0) "" >= int(-9223372036854775808)
unicode(0) "" < unicode(20) "-9223372036854775808"
int(1) < array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
int(1) >= unicode(0) ""
int(1) >= int(1)
int(1) < float(2.5)
int(1) >= int(0)
int(1) >= unicode(6) "string"
int(1) < unicode(3) "123"
int(1) < unicode(3) "2.5"
int(1) >= NULL
int(1) >= bool(true)
int(1) >= bool(false)
int(1)
Notice: Object of class stdClass could not be converted to int in %s on line %d
 >= object(stdClass)#%d (0) {}
int(1)
Notice: Object of class stdClass could not be converted to int in %s on line %d
 >= object(stdClass)#%d (0) {}
int(1)
Notice: Object of class test could not be converted to int in %s on line %d
 >= object(test)#%d (0) {}
int(1) < array(0) {}
int(1) >= int(-9223372036854775808)
int(1) >= unicode(20) "-9223372036854775808"
float(2.5) < array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
float(2.5) >= unicode(0) ""
float(2.5) >= int(1)
float(2.5) >= float(2.5)
float(2.5) >= int(0)
float(2.5) >= unicode(6) "string"
float(2.5) < unicode(3) "123"
float(2.5) >= unicode(3) "2.5"
float(2.5) >= NULL
float(2.5) >= bool(true)
float(2.5) >= bool(false)
float(2.5)
Notice: Object of class stdClass could not be converted to double in %s on line %d
 >= object(stdClass)#%d (0) {}
float(2.5)
Notice: Object of class stdClass could not be converted to double in %s on line %d
 >= object(stdClass)#%d (0) {}
float(2.5)
Notice: Object of class test could not be converted to double in %s on line %d
 >= object(test)#%d (0) {}
float(2.5) < array(0) {}
float(2.5) >= int(-9223372036854775808)
float(2.5) >= unicode(20) "-9223372036854775808"
int(0) < array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
int(0) >= unicode(0) ""
int(0) < int(1)
int(0) < float(2.5)
int(0) >= int(0)
int(0) >= unicode(6) "string"
int(0) < unicode(3) "123"
int(0) < unicode(3) "2.5"
int(0) >= NULL
int(0) < bool(true)
int(0) >= bool(false)
int(0)
Notice: Object of class stdClass could not be converted to int in %s on line %d
 < object(stdClass)#%d (0) {}
int(0)
Notice: Object of class stdClass could not be converted to int in %s on line %d
 < object(stdClass)#%d (0) {}
int(0)
Notice: Object of class test could not be converted to int in %s on line %d
 < object(test)#%d (0) {}
int(0) < array(0) {}
int(0) >= int(-9223372036854775808)
int(0) >= unicode(20) "-9223372036854775808"
unicode(6) "string" < array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
unicode(6) "string" >= unicode(0) ""
unicode(6) "string" < int(1)
unicode(6) "string" < float(2.5)
unicode(6) "string" >= int(0)
unicode(6) "string" >= unicode(6) "string"
unicode(6) "string" >= unicode(3) "123"
unicode(6) "string" >= unicode(3) "2.5"
unicode(6) "string" >= NULL
unicode(6) "string" >= bool(true)
unicode(6) "string" >= bool(false)
unicode(6) "string" < object(stdClass)#%d (0) {}
unicode(6) "string" < object(stdClass)#%d (0) {}
unicode(6) "string" < object(test)#%d (0) {}
unicode(6) "string" < array(0) {}
unicode(6) "string" >= int(-9223372036854775808)
unicode(6) "string" >= unicode(20) "-9223372036854775808"
unicode(3) "123" < array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
unicode(3) "123" >= unicode(0) ""
unicode(3) "123" >= int(1)
unicode(3) "123" >= float(2.5)
unicode(3) "123" >= int(0)
unicode(3) "123" < unicode(6) "string"
unicode(3) "123" >= unicode(3) "123"
unicode(3) "123" >= unicode(3) "2.5"
unicode(3) "123" >= NULL
unicode(3) "123" >= bool(true)
unicode(3) "123" >= bool(false)
unicode(3) "123" < object(stdClass)#%d (0) {}
unicode(3) "123" < object(stdClass)#%d (0) {}
unicode(3) "123" < object(test)#%d (0) {}
unicode(3) "123" < array(0) {}
unicode(3) "123" >= int(-9223372036854775808)
unicode(3) "123" >= unicode(20) "-9223372036854775808"
unicode(3) "2.5" < array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
unicode(3) "2.5" >= unicode(0) ""
unicode(3) "2.5" >= int(1)
unicode(3) "2.5" >= float(2.5)
unicode(3) "2.5" >= int(0)
unicode(3) "2.5" < unicode(6) "string"
unicode(3) "2.5" < unicode(3) "123"
unicode(3) "2.5" >= unicode(3) "2.5"
unicode(3) "2.5" >= NULL
unicode(3) "2.5" >= bool(true)
unicode(3) "2.5" >= bool(false)
unicode(3) "2.5" < object(stdClass)#%d (0) {}
unicode(3) "2.5" < object(stdClass)#%d (0) {}
unicode(3) "2.5" < object(test)#%d (0) {}
unicode(3) "2.5" < array(0) {}
unicode(3) "2.5" >= int(-9223372036854775808)
unicode(3) "2.5" >= unicode(20) "-9223372036854775808"
NULL < array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
NULL >= unicode(0) ""
NULL < int(1)
NULL < float(2.5)
NULL >= int(0)
NULL < unicode(6) "string"
NULL < unicode(3) "123"
NULL < unicode(3) "2.5"
NULL >= NULL
NULL < bool(true)
NULL >= bool(false)
NULL < object(stdClass)#%d (0) {}
NULL < object(stdClass)#%d (0) {}
NULL < object(test)#%d (0) {}
NULL >= array(0) {}
NULL < int(-9223372036854775808)
NULL < unicode(20) "-9223372036854775808"
bool(true) >= array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
bool(true) >= unicode(0) ""
bool(true) >= int(1)
bool(true) >= float(2.5)
bool(true) >= int(0)
bool(true) >= unicode(6) "string"
bool(true) >= unicode(3) "123"
bool(true) >= unicode(3) "2.5"
bool(true) >= NULL
bool(true) >= bool(true)
bool(true) >= bool(false)
bool(true) >= object(stdClass)#%d (0) {}
bool(true) >= object(stdClass)#%d (0) {}
bool(true) >= object(test)#%d (0) {}
bool(true) >= array(0) {}
bool(true) >= int(-9223372036854775808)
bool(true) >= unicode(20) "-9223372036854775808"
bool(false) < array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
bool(false) >= unicode(0) ""
bool(false) < int(1)
bool(false) < float(2.5)
bool(false) >= int(0)
bool(false) < unicode(6) "string"
bool(false) < unicode(3) "123"
bool(false) < unicode(3) "2.5"
bool(false) >= NULL
bool(false) < bool(true)
bool(false) >= bool(false)
bool(false) < object(stdClass)#%d (0) {}
bool(false) < object(stdClass)#%d (0) {}
bool(false) < object(test)#%d (0) {}
bool(false) >= array(0) {}
bool(false) < int(-9223372036854775808)
bool(false) < unicode(20) "-9223372036854775808"
object(stdClass)#%d (0) {} >= array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
object(stdClass)#%d (0) {} >= unicode(0) ""
object(stdClass)#%d (0) {}
Notice: Object of class stdClass could not be converted to int in %s on line %d
 >= int(1)
object(stdClass)#%d (0) {}
Notice: Object of class stdClass could not be converted to double in %s on line %d
 < float(2.5)
object(stdClass)#%d (0) {}
Notice: Object of class stdClass could not be converted to int in %s on line %d
 >= int(0)
object(stdClass)#%d (0) {} >= unicode(6) "string"
object(stdClass)#%d (0) {} >= unicode(3) "123"
object(stdClass)#%d (0) {} >= unicode(3) "2.5"
object(stdClass)#%d (0) {} >= NULL
object(stdClass)#%d (0) {} >= bool(true)
object(stdClass)#%d (0) {} >= bool(false)
object(stdClass)#%d (0) {} >= object(stdClass)#%d (0) {}
object(stdClass)#%d (0) {} >= object(stdClass)#%d (0) {}
object(stdClass)#%d (0) {} < object(test)#%d (0) {}
object(stdClass)#%d (0) {} >= array(0) {}
object(stdClass)#%d (0) {}
Notice: Object of class stdClass could not be converted to int in %s on line %d
 >= int(-9223372036854775808)
object(stdClass)#%d (0) {} >= unicode(20) "-9223372036854775808"
object(stdClass)#%d (0) {} >= array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
object(stdClass)#%d (0) {} >= unicode(0) ""
object(stdClass)#%d (0) {}
Notice: Object of class stdClass could not be converted to int in %s on line %d
 >= int(1)
object(stdClass)#%d (0) {}
Notice: Object of class stdClass could not be converted to double in %s on line %d
 < float(2.5)
object(stdClass)#%d (0) {}
Notice: Object of class stdClass could not be converted to int in %s on line %d
 >= int(0)
object(stdClass)#%d (0) {} >= unicode(6) "string"
object(stdClass)#%d (0) {} >= unicode(3) "123"
object(stdClass)#%d (0) {} >= unicode(3) "2.5"
object(stdClass)#%d (0) {} >= NULL
object(stdClass)#%d (0) {} >= bool(true)
object(stdClass)#%d (0) {} >= bool(false)
object(stdClass)#%d (0) {} >= object(stdClass)#%d (0) {}
object(stdClass)#%d (0) {} >= object(stdClass)#%d (0) {}
object(stdClass)#%d (0) {} < object(test)#%d (0) {}
object(stdClass)#%d (0) {} >= array(0) {}
object(stdClass)#%d (0) {}
Notice: Object of class stdClass could not be converted to int in %s on line %d
 >= int(-9223372036854775808)
object(stdClass)#%d (0) {} >= unicode(20) "-9223372036854775808"
object(test)#%d (0) {} >= array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
object(test)#%d (0) {} >= unicode(0) ""
object(test)#%d (0) {}
Notice: Object of class test could not be converted to int in %s on line %d
 >= int(1)
object(test)#%d (0) {}
Notice: Object of class test could not be converted to double in %s on line %d
 < float(2.5)
object(test)#%d (0) {}
Notice: Object of class test could not be converted to int in %s on line %d
 >= int(0)
object(test)#%d (0) {} >= unicode(6) "string"
object(test)#%d (0) {} >= unicode(3) "123"
object(test)#%d (0) {} >= unicode(3) "2.5"
object(test)#%d (0) {} >= NULL
object(test)#%d (0) {} >= bool(true)
object(test)#%d (0) {} >= bool(false)
object(test)#%d (0) {} < object(stdClass)#%d (0) {}
object(test)#%d (0) {} < object(stdClass)#%d (0) {}
object(test)#%d (0) {} >= object(test)#%d (0) {}
object(test)#%d (0) {} >= array(0) {}
object(test)#%d (0) {}
Notice: Object of class test could not be converted to int in %s on line %d
 >= int(-9223372036854775808)
object(test)#%d (0) {} >= unicode(20) "-9223372036854775808"
array(0) {} < array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
array(0) {} >= unicode(0) ""
array(0) {} >= int(1)
array(0) {} >= float(2.5)
array(0) {} >= int(0)
array(0) {} >= unicode(6) "string"
array(0) {} >= unicode(3) "123"
array(0) {} >= unicode(3) "2.5"
array(0) {} >= NULL
array(0) {} < bool(true)
array(0) {} >= bool(false)
array(0) {} < object(stdClass)#%d (0) {}
array(0) {} < object(stdClass)#%d (0) {}
array(0) {} < object(test)#%d (0) {}
array(0) {} >= array(0) {}
array(0) {} >= int(-9223372036854775808)
array(0) {} >= unicode(20) "-9223372036854775808"
int(-9223372036854775808) < array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
int(-9223372036854775808) < unicode(0) ""
int(-9223372036854775808) < int(1)
int(-9223372036854775808) < float(2.5)
int(-9223372036854775808) < int(0)
int(-9223372036854775808) < unicode(6) "string"
int(-9223372036854775808) < unicode(3) "123"
int(-9223372036854775808) < unicode(3) "2.5"
int(-9223372036854775808) >= NULL
int(-9223372036854775808) >= bool(true)
int(-9223372036854775808) >= bool(false)
int(-9223372036854775808)
Notice: Object of class stdClass could not be converted to int in %s on line %d
 < object(stdClass)#%d (0) {}
int(-9223372036854775808)
Notice: Object of class stdClass could not be converted to int in %s on line %d
 < object(stdClass)#%d (0) {}
int(-9223372036854775808)
Notice: Object of class test could not be converted to int in %s on line %d
 < object(test)#%d (0) {}
int(-9223372036854775808) < array(0) {}
int(-9223372036854775808) >= int(-9223372036854775808)
int(-9223372036854775808) >= unicode(20) "-9223372036854775808"
unicode(20) "-9223372036854775808" < array(3) {  [0]=>  int(1)  [1]=>  int(2)  [2]=>  int(3)}
unicode(20) "-9223372036854775808" >= unicode(0) ""
unicode(20) "-9223372036854775808" < int(1)
unicode(20) "-9223372036854775808" < float(2.5)
unicode(20) "-9223372036854775808" < int(0)
unicode(20) "-9223372036854775808" < unicode(6) "string"
unicode(20) "-9223372036854775808" < unicode(3) "123"
unicode(20) "-9223372036854775808" < unicode(3) "2.5"
unicode(20) "-9223372036854775808" >= NULL
unicode(20) "-9223372036854775808" >= bool(true)
unicode(20) "-9223372036854775808" >= bool(false)
unicode(20) "-9223372036854775808" < object(stdClass)#%d (0) {}
unicode(20) "-9223372036854775808" < object(stdClass)#%d (0) {}
unicode(20) "-9223372036854775808" < object(test)#%d (0) {}
unicode(20) "-9223372036854775808" < array(0) {}
unicode(20) "-9223372036854775808" >= int(-9223372036854775808)
unicode(20) "-9223372036854775808" >= unicode(20) "-9223372036854775808"
Done
