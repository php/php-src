--TEST--
Test ob_start() with object supplied but no method.
--FILE--
<?php
/*
 * Function is implemented in main/output.c
*/

Class C {
}

$c = new C;
try {
    var_dump(ob_start(array($c)));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo "done"
?>
--EXPECT--
TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, array callback must have exactly two members
done
