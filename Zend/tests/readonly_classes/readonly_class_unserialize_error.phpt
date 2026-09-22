--TEST--
Fix GH-9186 Readonly classes can have dynamic properties created by unserialize()
--FILE--
<?php

readonly class C {}

try {
    $readonly = unserialize('O:1:"C":1:{s:1:"x";b:1;}');
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot create dynamic property C::$x
