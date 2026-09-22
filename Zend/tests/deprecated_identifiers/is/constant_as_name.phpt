--TEST--
Using "is" as a constant name is deprecated
--FILE--
<?php

namespace Foo\Bar {
    const is = 5;
}

namespace {
    const is = 5;

    define('is', 'x');

    define('foo\\is', 'x');
}

?>
--EXPECTF--
Deprecated: Calling a constant "is" is deprecated since 8.6 in %s on line %d

Deprecated: Calling a constant "is" is deprecated since 8.6 in %s on line %d

Deprecated: Calling a constant "is" is deprecated since 8.6 in %s on line %d

Warning: Constant is already defined, this will be an error in PHP 9 in %s on line %d

Deprecated: Calling a constant "is" is deprecated since 8.6 in %s on line %d
