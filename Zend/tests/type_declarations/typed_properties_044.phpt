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

/* (1 << 63) - 1 overflows to float, which is exactly convertible back to (1 << 63) */
$bar = PHP_INT_MIN;

var_dump($bar--);
var_dump(--$bar);

?>
--EXPECT--
int(0)
int(-2)
int(-1)
int(-1)
Cannot assign float to reference held by property class@anonymous->bar of type ?int
Cannot assign float to reference held by property class@anonymous->bar of type ?int
int(-9223372036854775808)
int(-9223372036854775808)
