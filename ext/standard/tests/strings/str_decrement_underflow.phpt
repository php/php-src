--TEST--
str_decrement(): Out of Range ValueErrors for strings that cannot be decremented
--FILE--
<?php

$strings = [
    "",
    "0",
    "a",
    "A",
    "00",
    "0a",
    "0A",
];

foreach ($strings as $s) {
    try {
        var_dump(str_decrement($s));
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
str_decrement(): Argument #1 ($string) cannot be empty
str_decrement(): Argument #1 ($string) "0" is out of decrement range
str_decrement(): Argument #1 ($string) "a" is out of decrement range
str_decrement(): Argument #1 ($string) "A" is out of decrement range
str_decrement(): Argument #1 ($string) "00" is out of decrement range
str_decrement(): Argument #1 ($string) "0a" is out of decrement range
str_decrement(): Argument #1 ($string) "0A" is out of decrement range
