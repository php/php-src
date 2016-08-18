--TEST--
Bug #40509 (key() function changed behaviour if global array is used within function)
--FILE--
<?php
function foo()
{
	global $arr;
	
	$c = $arr["v"];
	foreach ($c as $v) {}
}

$arr["v"] = array("a");

var_dump(key($arr["v"]));
foo();
var_dump(key($arr["v"]));
foreach ($arr["v"] as $k => $v) {
	var_dump($k);
}
var_dump(key($arr["v"]));
--EXPECT--
int(0)
int(0)
int(0)
int(0)
