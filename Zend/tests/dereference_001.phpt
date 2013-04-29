--TEST--
Testing array dereference
--FILE--
<?php
error_reporting(E_ALL);

function a() {
	return array(1,array(5));
}
var_dump(a()[1][0]); // int(5)

function b() {
	return array();
}
var_dump(b()[0]); // Notice: Undefined offset: 0 

class foo {
	public $y = 1; 
	
	public function test() {
		return array(array(array('foobar')));
	}
}

function c() {
	return array(new foo);
}
var_dump(c()[0]->y); // int(1)

function d() {
	$obj = new foo;
	return $obj->test();
}
var_dump(d()[0][0][0][3]); // string(1) "b"

function e() {
	$y = 'bar';
	$x = array('a' => 'foo', 'b' => $y);
	return $x;
}
var_dump(e()['b']); // string(3) "bar"

?>
--EXPECTF--
int(5)

Notice: Undefined offset: 0 in %s on line %d
NULL
int(1)
string(1) "b"
string(3) "bar"
