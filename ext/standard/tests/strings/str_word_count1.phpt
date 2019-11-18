--TEST--
str_word_count() and invalid arguments
--FILE--
<?php

var_dump(str_word_count(""));

try {
    var_dump(str_word_count("", -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(str_word_count("", -1, $a));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

var_dump($a);
?>
--EXPECTF--
int(0)
Invalid format value -1

Warning: Undefined variable: a in %s on line %d
Invalid format value -1

Warning: Undefined variable: a in %s on line %d
NULL
