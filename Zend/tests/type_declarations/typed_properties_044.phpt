--TEST--
Test increment functions on typed property references
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
Cannot assign float to reference of type ?integer
Cannot assign float to reference of type ?integer
Cannot assign float to reference of type ?integer
Cannot assign float to reference of type ?integer

