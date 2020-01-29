--TEST--
strrpos() offset integer overflow
--FILE--
<?php

try {
    var_dump(strrpos("t", "t", PHP_INT_MAX+1));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    strrpos(1024, 1024, -PHP_INT_MAX);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    strrpos(1024, "te", -PHP_INT_MAX);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    strrpos(1024, 1024, -PHP_INT_MAX-1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    strrpos(1024, "te", -PHP_INT_MAX-1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECT--
strrpos() expects argument #3 ($offset) to be of type int, float given
Offset not contained in string
Offset not contained in string
Offset not contained in string
Offset not contained in string
Done
