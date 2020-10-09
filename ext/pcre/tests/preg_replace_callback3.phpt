--TEST--
preg_replace_callback() 3
--FILE--
<?php

try {
    var_dump(preg_replace_callback(1,2,3));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(preg_replace_callback(1,2,3,4));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$a = 5;
try {
    var_dump(preg_replace_callback(1,2,3,4,$a));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
preg_replace_callback(): Argument #2 ($callback) must be a valid callback, no array or string given
preg_replace_callback(): Argument #2 ($callback) must be a valid callback, no array or string given
preg_replace_callback(): Argument #2 ($callback) must be a valid callback, no array or string given
