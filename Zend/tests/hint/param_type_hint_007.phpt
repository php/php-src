--TEST--
Parameter type hint - Testing parameter with reference
--FILE--
<?php

/* string */
function test_string(string &$a) {
	$a .= '!!!';
}

$str = 'foo';
test_string($str);
var_dump($str);

/* integer */
function test_int(int &$b) {
	$b += 2;
}

$b = 2;
test_int($b);
var_dump($b);

/* object */
function test_obj(object &$c) {
	$c->tested = 1;
}
$c = new stdclass;
test_obj($c);
var_dump($c);

?>
--EXPECT--
string(6) "foo!!!"
int(4)
object(stdClass)#1 (1) {
  ["tested"]=>
  int(1)
}
--UEXPECT--
unicode(6) "foo!!!"
int(4)
object(stdClass)#1 (1) {
  [u"tested"]=>
  int(1)
}
