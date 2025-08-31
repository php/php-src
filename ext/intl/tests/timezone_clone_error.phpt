--TEST--
IntlTimeZone clone handler: error test
--EXTENSIONS--
intl
--FILE--
<?php

class A extends IntlTimeZone {
    public function __construct() {}
}

$tz = new A();
try {
    $b = clone $tz;
    var_dump($b);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Cannot clone uninitialized IntlTimeZone
