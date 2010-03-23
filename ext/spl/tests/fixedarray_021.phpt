--TEST--
SPL: FixedArray: misc small tests
--FILE--
<?php

/* empty count */
$a = new SplFixedArray();

var_dump(count($a));
var_dump($a->count());

/* negative init value */
try {
	$b = new SplFixedArray(-10);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

/* resize and negative value */
$b = new SplFixedArray();
try {
	$b->setSize(-5);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

/* calling __construct() twice */
$c = new SplFixedArray(0);
var_dump($c->__construct());

/* fromArray() from empty array */
$d = new SplFixedArray();
$d->fromArray(array());

var_dump(count($a));
var_dump($a->count());
var_dump($a);

/* foreach by ref */
$e = new SplFixedArray(10);
$e[0] = 1;
$e[1] = 5;
$e[2] = 10;

try {
	foreach ($e as $k=>&$v) {
		var_dump($v);
	}
} catch (Exception $e) {
	var_dump($e->getMessage());
}

//non-long indexes
$a = new SplFixedArray(4);
$a["2"] = "foo";
$a["1"] = "foo";
$a["3"] = "0";

var_dump(isset($a["0"], $a[-1]), $a["1"]);
var_dump(empty($a["3"]));

?>
==DONE==
--EXPECTF--
int(0)
int(0)
string(35) "array size cannot be less than zero"
string(35) "array size cannot be less than zero"
NULL
int(0)
int(0)
object(SplFixedArray)#%d (0) {
}
string(52) "An iterator cannot be used with foreach by reference"
bool(false)
string(3) "foo"
bool(true)
==DONE==
