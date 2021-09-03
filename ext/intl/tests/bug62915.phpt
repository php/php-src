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
} catch (Exception $e) {
        var_dump($e->getMessage());
}
?>
--EXPECT--
string(39) "Cannot clone unconstructed IntlTimeZone"
