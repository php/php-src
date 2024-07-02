--TEST--
Test ob_start() with non existent callback method.
--FILE--
<?php
/*
 * Function is implemented in main/output.c
*/

Class C {
}

$c = new C;
try {
    var_dump(ob_start(array($c, 'f')));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo "done"
?>
--EXPECT--
TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, class C does not have a method "f"
done
