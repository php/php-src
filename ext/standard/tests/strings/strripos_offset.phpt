--TEST--
strripos() offset integer overflow
--FILE--
<?php

try {
    var_dump(strripos("t", "t", PHP_INT_MAX+1));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

var_dump(strripos(1024, 1024, -PHP_INT_MAX));
var_dump(strripos(1024, "te", -PHP_INT_MAX));
var_dump(strripos(1024, 1024, -PHP_INT_MAX-1));
var_dump(strripos(1024, "te", -PHP_INT_MAX-1));

echo "Done\n";
?>
--EXPECTF--
strripos() expects parameter 3 to be int, float given

Warning: strripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: strripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: strripos(): Offset not contained in string in %s on line %d
bool(false)

Warning: strripos(): Offset not contained in string in %s on line %d
bool(false)
Done
