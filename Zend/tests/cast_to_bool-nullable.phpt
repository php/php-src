--TEST--
casting different variables to nullable boolean
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
	$tmp = (?bool)$var;
	var_dump($tmp);
}

$tmp = (?bool)$undefined_var;
var_dump($tmp);

$unset_var = 10;
unset($unset_var);
$tmp = (?bool)$unset_var;
var_dump($tmp);

$ref_to_new_var = &$new_var;
$ref_to_ref_to_new_var = &$ref_to_new_var;
$tmp = (?bool)$ref_to_ref_to_new_var;
var_dump($tmp);

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
NULL
bool(true)
bool(true)

Notice: Undefined variable: undefined_var in %s on line %d
NULL

Notice: Undefined variable: unset_var in %s on line %d
NULL
NULL
Done
