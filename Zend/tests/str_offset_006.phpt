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
--EXPECT--
Err: Undefined variable $y (this will become an error in PHP 9.0)
Err: Undefined variable $y (this will become an error in PHP 9.0)
Err: String offset cast occurred
NULL
