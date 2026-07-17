--TEST--
GH-21245 (Use-after-free in ASSIGN_DIM when the error handler grows the array holding a typed-property reference)
--FILE--
<?php
class Test {
    public string $x;
}
$a = [];
set_error_handler(function() use (&$a) {
    $a['b'] = 2;
});
$test = new Test;
$test->x = "";
$a[0] =& $test->x;
var_dump($a[0] = $v);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot assign null to reference held by property Test::$x of type string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
