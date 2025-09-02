--TEST--
Cannot use instance of Directory class constructed via Reflection.
--FILE--
<?php

$rc = new ReflectionClass("Directory");
var_dump($rc->isInstantiable());
try {
    $d = $rc->newInstanceWithoutConstructor();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

var_dump($d);
try {
    var_dump($d->read());
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
bool(true)
object(Directory)#2 (0) {
  ["path"]=>
  uninitialized(string)
  ["handle"]=>
  uninitialized(mixed)
}
Error: Internal directory stream has been altered
