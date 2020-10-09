--TEST--
Type group use declarations should not allow override on inner items
--FILE--
<?php

// should throw syntax errors

use const Foo\Bar\{
    A,
    const B,
    function C
};
?>
--EXPECTF--
Parse error: syntax error, unexpected token "const", expecting "}" in %s on line %d
