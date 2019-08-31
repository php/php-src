--TEST--
Test increment functions on typed property references
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64 bit platform only"); ?>
--FILE--
<?php
$foo = new class {
	public ?int $bar;
};

$bar = &$foo->bar;

$bar *= 1;

var_dump($bar--);
var_dump(--$bar);
var_dump(++$bar);
var_dump($bar++);

$bar = PHP_INT_MAX;

try {
	var_dump($bar++);
} catch (Throwable $e) {
	echo $e->getMessage() . "\n";
}

try {
	var_dump(++$bar);
} catch (Throwable $e) {
	echo $e->getMessage() . "\n";
}

$bar = PHP_INT_MIN;


try {
	var_dump($bar--);
} catch (Throwable $e) {
	echo $e->getMessage() . "\n";
}

try {
	var_dump(--$bar);
} catch (Throwable $e) {
	echo $e->getMessage() . "\n";
}

?>
--EXPECT--
int(0)
int(-2)
int(-1)
int(-1)
Cannot increment a reference held by property class@anonymous::$bar of type ?int past its maximal value
Cannot increment a reference held by property class@anonymous::$bar of type ?int past its maximal value
Cannot decrement a reference held by property class@anonymous::$bar of type ?int past its minimal value
Cannot decrement a reference held by property class@anonymous::$bar of type ?int past its minimal value
