--TEST--
casting different variables to double 
--INI--
precision=14
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
	$tmp = (double)$var;
	var_dump($tmp);
}

echo "Done\n";
?>
--EXPECTF--	
float(0)
float(8754456)
float(0)
float(0)
float(9876545)
float(0.1)
float(0)
float(1)
float(0)
float(1)
float(0)
float(%d)

Notice: Object of class test could not be converted to float in %s on line %d
float(1)
Done
