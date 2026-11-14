--TEST--
Csv\LazyLaxCollection must not be instantiable
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$rc = new ReflectionClass('Csv\LazyLaxCollection');
var_dump($rc);

try {
    $o = $rc->newInstance();
    var_dump($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
    $o = $rc->newInstanceArgs();
    var_dump($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
    $o = $rc->newInstanceWithoutConstructor();
    var_dump($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
object(ReflectionClass)#1 (1) {
  ["name"]=>
  string(21) "Csv\LazyLaxCollection"
}
Error: Cannot instantiate Csv\LazyLaxCollection class
Error: Cannot instantiate Csv\LazyLaxCollection class
ReflectionException: Class Csv\LazyLaxCollection is an internal class marked as final that cannot be instantiated without invoking its constructor
