--TEST--
Test isset() function : basic functionality
--FILE--
<?php
class foo {}

echo "*** Testing isset() : basic functionality ***\n";

$i = 10;
$f = 10.5;
$s = "Hello";
$b = true;
$n = NULL;

echo "Test multiple scalar variables in a group\n";
var_dump(isset($i, $f, $s, $b));
var_dump(isset($i, $f, $s, $b, $n));

echo "Unset a few\n";
unset($i, $b);

echo "Test again\n";
var_dump(isset($i, $f, $s, $b));

echo "\n\nArray test:\n";
$arr = array();
var_dump(isset($var));
var_dump(isset($var[1]));
var_dump(isset($var, $var[1]));
echo "..now set\n";
$var[1] = 10;
var_dump(isset($var));
var_dump(isset($var[1]));
var_dump(isset($var, $var[1]));

?>
--EXPECT--
*** Testing isset() : basic functionality ***
Test multiple scalar variables in a group
bool(true)
bool(false)
Unset a few
Test again
bool(false)


Array test:
bool(false)
bool(false)
bool(false)
..now set
bool(true)
bool(true)
bool(true)
