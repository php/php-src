--TEST--
Test sprintf() function : Rope Optimization
--FILE--
<?php
function func($str) {
	return strtoupper($str);
}
function sideeffect() {
	echo "Called!\n";
	return "foo";
}
class Foo {
	public function __construct() {
		echo "Called\n";
	}
}

$a = "foo";
$b = "bar";
$c = new stdClass();

try {
	var_dump(sprintf("const"));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%s", $a));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%s/%s", $a, $b));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%s/%s/%s", $a, $b));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%s/%s/%s", $a, $b, $c));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%s/", func("baz")));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("/%s", func("baz")));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("/%s/", func("baz")));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%s%s%s%s", $a, $b, func("baz"), $a));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%s/%s", sprintf("%s:%s", $a, $b), sprintf("%s-%s", func('baz'), func('baz'))));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf(sideeffect()));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%s-%s-%s", __FILE__, __LINE__, 1));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	$values = range('a', 'z');
	var_dump(sprintf("%s%s%s", "{$values[0]}{$values[1]}{$values[2]}", "{$values[3]}{$values[4]}{$values[5]}", "{$values[6]}{$values[7]}{$values[8]}"));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%s%s%s", new Foo(), new Foo(), new Foo(), ));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf(...));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf('%%s'));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf('%%s', 'test'));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf('%s-%s-%s', [], [], []));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf(""));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf());
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf('%s-%s-%s', true, false, true));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

echo "Done";
?>
--EXPECTF--
string(5) "const"

string(3) "foo"

string(7) "foo/bar"

ArgumentCountError: 4 arguments are required, 3 given in %s:32
Stack trace:
#0 %s(32): sprintf('%s/%s/%s', 'foo', 'bar')
#1 {main}

Error: Object of class stdClass could not be converted to string in %s:36
Stack trace:
#0 {main}

string(4) "BAZ/"

string(4) "/BAZ"

string(5) "/BAZ/"

string(12) "foobarBAZfoo"

string(15) "foo:bar/BAZ-BAZ"

Called!
string(3) "foo"

string(%d) "%ssprintf_rope_optimization_001.php-%d-1"

string(9) "abcdefghi"

Called
Called
Called
Error: Object of class Foo could not be converted to string in %s:73
Stack trace:
#0 {main}

object(Closure)#3 (2) {
  ["function"]=>
  string(7) "sprintf"
  ["parameter"]=>
  array(2) {
    ["$format"]=>
    string(10) "<required>"
    ["$values"]=>
    string(10) "<optional>"
  }
}

string(2) "%s"

string(2) "%s"


Warning: Array to string conversion in %s on line 89

Warning: Array to string conversion in %s on line 89

Warning: Array to string conversion in %s on line 89
string(17) "Array-Array-Array"

string(0) ""

ArgumentCountError: sprintf() expects at least 1 argument, 0 given in %s:97
Stack trace:
#0 %s(97): sprintf()
#1 {main}

string(4) "1--1"

Done
