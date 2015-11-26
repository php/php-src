--TEST--
casting different variables to boolean using settype()
--FILE--
<?php

$r = fopen(__FILE__, "r");

class test {
	function  __toString() {
		return "10";
	}
}

$o = new test;

$vars = array(
	"string",
	"8754456",
	"",
	"\0",
	9876545,
	0.10,
	array(),
	array(1,2,3),
	false,
	true,
	NULL,
	$r,
	$o
);

foreach ($vars as $var) {
	settype($var, "bool");
	var_dump($var);
}

echo "Done\n";
?>
--EXPECTF--	
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
Done
