--TEST--
Test short ternary operator for implicit isset cases
--FILE--
<?php

$var = 7;
$var2 = false;

$obj = new StdClass;
$obj->boo = 7;

$arr = [
	2 => 7,
	"foo" => "bar",
	"foobar" => "",
	"qux" => $obj,
	"bing" => [
		"bang"
	]
];

var_dump($nonexistant_variable ?: 3);
echo PHP_EOL;
var_dump($var ?: 3);
var_dump($var2 ?: 3);
echo PHP_EOL;
var_dump($obj->boo ?: 3);
var_dump($obj->bing ?: 3);
var_dump($arr["qux"]->boo ?: 3);
var_dump($arr["qux"]->bing ?: 3);
echo PHP_EOL;
var_dump($arr[2] ?: 3);
var_dump($arr["foo"] ?: 3);
var_dump($arr["foobar"] ?: 3);
var_dump($arr["qux"] ?: 3);
var_dump($arr["bing"][0] ?: 3);
var_dump($arr["bing"][1] ?: 3);
?>
--EXPECTF--
int(3)

int(7)
int(3)

int(7)
int(3)
int(7)
int(3)

int(7)
string(3) "bar"
int(3)
object(stdClass)#1 (%d) {
  ["boo"]=>
  int(7)
}
string(4) "bang"
int(3)