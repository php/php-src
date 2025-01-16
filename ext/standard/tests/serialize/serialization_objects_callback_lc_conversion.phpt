--TEST--
Bad unserialize_callback_func
--FILE--
<?php
ini_set('unserialize_callback_func','PascalCase');

function PascalCase(string $class) {
    var_dump($class);
}

try {
    unserialize('O:3:"FOO":0:{}');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
string(3) "FOO"

Warning: unserialize(): Function PascalCase() hasn't defined the class it was called for in %s on line %d
