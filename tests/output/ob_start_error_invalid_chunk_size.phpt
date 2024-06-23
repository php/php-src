--TEST--
ob_start(): ensure buffers can't be added from within callback.
--FILE--
<?php

/*
 * Function is implemented in main/output.c
*/

function f($str) {
    echo "hello";
    return $str;
}

try {
    var_dump(ob_start('f', -15));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo "done";
?>
--EXPECT--
bool(true)
done
