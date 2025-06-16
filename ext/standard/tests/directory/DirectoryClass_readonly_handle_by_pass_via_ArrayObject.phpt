--TEST--
Changing Directory::$handle property
--FILE--
<?php

$d = dir(__DIR__);
$ao = new ArrayObject($d);

try {
    $ao['handle'] = STDERR;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($d->handle);

try {
    $s = $d->read();
    var_dump($s);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    unset($ao['handle']);
    var_dump($d->handle);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
resource(3) of type (stream)
Error: Internal directory stream has been altered
Error: Typed property Directory::$handle must not be accessed before initialization
