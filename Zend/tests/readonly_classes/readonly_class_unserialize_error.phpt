--TEST--
Fix GH-9186 Readonly classes can have dynamic properties created by unserialize()
--FILE--
<?php

readonly class C {}

try {
    $readonly = unserialize('O:1:"C":1:{s:1:"x";b:1;}');
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot create dynamic property C::$x
