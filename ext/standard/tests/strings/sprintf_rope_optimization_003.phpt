--TEST--
Test sprintf() function : Rope Optimization for '%d'.
--FILE--
<?php
function func($num) {
	return $num + 1;
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

$a = 42;
$b = -1337;
$c = 3.14;
$d = new stdClass();

try {
	var_dump(sprintf("%d", $a));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%d/%d", $a, $b));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%d/%d/%d", $a, $b, $c));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%d/%d/%d/%d", $a, $b, $c, $d));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%d/", func(0)));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("/%d", func(0)));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("/%d/", func(0)));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%d/%d/%d/%d", $a, $b, func(0), $a));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%d/%d/%d/%d", __FILE__, __LINE__, 1, M_PI));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%d/%d/%d", new Foo(), new Foo(), new Foo(), ));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf('%d/%d/%d', [], [], []));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	if (PHP_INT_SIZE == 8) {
		var_dump(sprintf('%d/%d/%d', PHP_INT_MAX, 0, PHP_INT_MIN));
		var_dump("2147483647/0/-2147483648");
	} else {
		var_dump("9223372036854775807/0/-9223372036854775808");
		var_dump(sprintf('%d/%d/%d', PHP_INT_MAX, 0, PHP_INT_MIN));
	}
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf('%d/%d/%d', true, false, true));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%d/%d", true, 'foo'));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

try {
	var_dump(sprintf("%d", 'foo'));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

echo "Done";
?>
--EXPECTF--
string(2) "42"

string(8) "42/-1337"

string(10) "42/-1337/3"


Warning: Object of class stdClass could not be converted to int in %s on line 33
string(12) "42/-1337/3/1"

string(2) "1/"

string(2) "/1"

string(3) "/1/"

string(13) "42/-1337/1/42"

string(8) "0/53/1/3"

Called
Called
Called

Warning: Object of class Foo could not be converted to int in %s on line 57

Warning: Object of class Foo could not be converted to int in %s on line 57

Warning: Object of class Foo could not be converted to int in %s on line 57
string(5) "1/1/1"

string(5) "0/0/0"

string(42) "9223372036854775807/0/-9223372036854775808"
string(24) "2147483647/0/-2147483648"

string(5) "1/0/1"

string(3) "1/0"

string(1) "0"

Done
