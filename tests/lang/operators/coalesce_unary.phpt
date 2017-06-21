--TEST--
Test ?? operator (unary)
--FILE--
<?php

$var = 7;
$var2 = NULL;

$obj = new StdClass;
$obj->boo = 7;

$arr = [
	2 => 7,
	"foo" => "bar",
	"foobar" => NULL,
	"qux" => $obj,
	"bing" => [
		"bang"
	]
];

function foobar() {
	echo "called\n";
	return ['a'];
}

var_dump($nonexistent_variable??);
echo PHP_EOL;
var_dump($var??);
var_dump($var2??);
echo PHP_EOL;
var_dump($obj->boo??);
var_dump($obj->bing??);
var_dump($arr["qux"]->boo??);
var_dump($arr["qux"]->bing??);
echo PHP_EOL;
var_dump($arr[2]??);
var_dump($arr["foo"]??);
var_dump($arr["foobar"]??);
var_dump($arr["qux"]??);
var_dump($arr["bing"][0]??);
var_dump($arr["bing"][1]??);
echo PHP_EOL;
var_dump(foobar()[0]??);
echo PHP_EOL;
function f($x)
{
    printf("%s(%d)\n", __FUNCTION__, $x);
    return $x;
}

$a = f(null)??;
?>
--EXPECTF--
NULL

int(7)
NULL

int(7)
NULL
int(7)
NULL

int(7)
string(3) "bar"
NULL
object(stdClass)#%d (%d) {
  ["boo"]=>
  int(7)
}
string(4) "bang"
NULL

called
string(1) "a"

f(0)
