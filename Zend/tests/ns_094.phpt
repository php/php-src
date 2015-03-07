--TEST--
Type group use declarations should not allow override on inner itens
--FILE--
<?php

// should not throw syntax errors

use const Foo\Bar\{
    A,
    const B,
    function C
};

--EXPECTF--

Parse error: syntax error, unexpected 'const' (T_CONST), expecting identifier (T_STRING) or \\ (T_NS_SEPARATOR) in %s on line 7
