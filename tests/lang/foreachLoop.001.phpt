--TEST--
Foreach loop tests - basic loop with just value and key => value.
--FILE--
<?php

$a = array("a","b","c");

foreach ($a as $v) {
	var_dump($v);
}
foreach ($a as $k => $v) {
	var_dump($k, $v);
}
//check key and value after the loop.
var_dump($k, $v);

echo "\n";
//Dynamic array
foreach (array("d","e","f") as $v) {
	var_dump($v);
}
foreach (array("d","e","f") as $k => $v) {
	var_dump($k, $v);
}
//check key and value after the loop.
var_dump($k, $v);

echo "\n";
//Ensure counter is advanced during loop
$a=array("a","b","c");
foreach ($a as $v);
var_dump(current($a));
$a=array("a","b","c");
foreach ($a as &$v);
var_dump(current($a));

?>
--EXPECT--
unicode(1) "a"
unicode(1) "b"
unicode(1) "c"
int(0)
unicode(1) "a"
int(1)
unicode(1) "b"
int(2)
unicode(1) "c"
int(2)
unicode(1) "c"

unicode(1) "d"
unicode(1) "e"
unicode(1) "f"
int(0)
unicode(1) "d"
int(1)
unicode(1) "e"
int(2)
unicode(1) "f"
int(2)
unicode(1) "f"

bool(false)
bool(false)