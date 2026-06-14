--TEST--
First Class Callable from Special Compiler Function
--FILE--
<?php
$strlen = strlen(...);

if ($strlen("Hello World") === 11) {
    echo "OK";
}
?>
--EXPECT--
OK
