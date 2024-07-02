--TEST--
Test wrong number of arguments and wrong arg types for ob_start()
--FILE--
<?php
/*
 * Function is implemented in main/output.c
*/

Class C {
    static function f($str) {
        return $str;
    }
}

try {
    var_dump(ob_start(array("nonExistent","f")));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(ob_start(array("C","nonExistent")));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(ob_start("C::no"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(ob_start("no"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo "done"
?>
--EXPECT--
TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, class "nonExistent" not found
TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, class C does not have a method "nonExistent"
TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, class C does not have a method "no"
TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, function "no" not found or invalid function name
done
