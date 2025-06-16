--TEST--
Crash with specific assertions and zend.assertions=-1
--INI--
zend.assertions=-1
--FILE--
<?php

function f() {
    assert(@$a ?: 1);
    echo "OK";
};
f();

?>
--EXPECT--
OK
