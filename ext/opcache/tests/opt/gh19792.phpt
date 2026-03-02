--TEST--
GH-19792 (SCCP causes UAF for return value if both warning and exception are triggered)
--EXTENSIONS--
opcache
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php

function foo()
{
    return \zend_test_gh19792();
}

try {
    foo();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: a warning in %s on line %d
an exception
