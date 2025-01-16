--TEST--
unserialize_callback_func which contains null bytes
--FILE--
<?php
ini_set('unserialize_callback_func', "foo\0butno");

function foo(string $name) {
    echo "Haha";
}

try {
    unserialize('O:3:"FOO":0:{}');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Haha
Warning: unserialize(): Function foo() hasn't defined the class it was called for in %s on line %d
