--TEST--
strrpos() offset integer overflow
--FILE--
<?php

try {
    var_dump(strrpos("t", "t", PHP_INT_MAX+1));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

var_dump(strrpos(1024, 1024, -PHP_INT_MAX));
var_dump(strrpos(1024, "te", -PHP_INT_MAX));
var_dump(strrpos(1024, 1024, -PHP_INT_MAX-1));
var_dump(strrpos(1024, "te", -PHP_INT_MAX-1));

echo "Done\n";
?>
--EXPECTF--
strrpos() expects parameter 3 to be int, float given

Warning: strrpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: strrpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: strrpos(): Offset not contained in string in %s on line %d
bool(false)

Warning: strrpos(): Offset not contained in string in %s on line %d
bool(false)
Done
