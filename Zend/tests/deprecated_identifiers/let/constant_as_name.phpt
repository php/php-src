--TEST--
Using "let" as a constant name is deprecated
--FILE--
<?php

namespace Foo\Bar {
    const let = 5;
}

namespace {
    const let = 5;

    define('let', 'x');

    define('foo\\let', 'x');
}

?>
--EXPECTF--
Deprecated: Calling a constant "let" is deprecated since 8.6 in %s on line %d

Deprecated: Calling a constant "let" is deprecated since 8.6 in %s on line %d

Deprecated: Calling a constant "let" is deprecated since 8.6 in %s on line %d

Warning: Constant let already defined, this will be an error in PHP 9 in %s on line %d

Deprecated: Calling a constant "let" is deprecated since 8.6 in %s on line %d
