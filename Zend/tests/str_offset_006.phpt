--TEST--
string offset 006 indirect string modification by error handler
--FILE--
<?php
set_error_handler(function($code, $msg) {
    echo "Err: $msg\n";
    $GLOBALS['a']=null;
});
$a[$y]=$a.=($y);
var_dump($a);
?>
--EXPECTF--
Err: Undefined variable $y
Err: Undefined variable $a
Err: Undefined variable $y
Err: String offset cast occurred

Fatal error: Uncaught Error: Cannot assign an empty string to a string offset in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d