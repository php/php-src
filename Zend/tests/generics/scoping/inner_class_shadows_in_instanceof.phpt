--TEST--
Scoping: inner class shadows the type parameter for `instanceof`
--FILE--
<?php
function f<T>($x): bool {
    class T {}
    return $x instanceof T;
}
$called = false;
$result = f(new stdClass);
var_dump($result);
?>
--EXPECT--
bool(false)
