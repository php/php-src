--TEST--
casting different variables to nullable integer
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
	$tmp = (?int)$var;
	var_dump($tmp);
}

$tmp = (?int)$undefined_var;
var_dump($tmp);

$unset_var = 10;
unset($unset_var);
$tmp = (?int)$unset_var;
var_dump($tmp);

$ref_to_new_var = &$new_var;
$ref_to_ref_to_new_var = &$ref_to_new_var;
$tmp = (?int)$ref_to_ref_to_new_var;
var_dump($tmp);

echo "Done\n";
?>
--EXPECTF--
int(0)
int(8754456)
int(0)
int(0)
int(9876545)
int(0)
int(0)
int(1)
int(0)
int(1)
NULL
int(%d)

Notice: Object of class test could not be converted to int in %s on line %d
int(1)

Notice: Undefined variable: undefined_var in %s on line %d
NULL

Notice: Undefined variable: unset_var in %s on line %d
NULL
NULL
Done
