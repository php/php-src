--TEST--
casting different variables to nullable double
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
	$tmp = (?double)$var;
	var_dump($tmp);
}

$tmp = (?double)$undefined_var;
var_dump($tmp);

$unset_var = 10;
unset($unset_var);
$tmp = (?double)$unset_var;
var_dump($tmp);

$ref_to_new_var = &$new_var;
$ref_to_ref_to_new_var = &$ref_to_new_var;
$tmp = (?double)$ref_to_ref_to_new_var;
var_dump($tmp);

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
NULL
float(%d)

Notice: Object of class test could not be converted to float in %s on line %d
float(1)

Notice: Undefined variable: undefined_var in %s on line %d
NULL

Notice: Undefined variable: unset_var in %s on line %d
NULL
NULL
Done
