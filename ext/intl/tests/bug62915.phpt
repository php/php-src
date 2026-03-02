--TEST--
Bug #62915: incomplete cloning of IntlTimeZone objects
--EXTENSIONS--
intl
--FILE--
<?php

class foo extends IntlTimeZone {
        public $foo = 'test';
        public function __construct() { }
}

$x = new foo;

try {
    $z = clone $x;
    var_dump($z);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Error: Cannot clone uninitialized IntlTimeZone
