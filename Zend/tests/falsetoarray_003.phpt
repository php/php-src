--TEST--
Autovivification of false to array with data clobbering by error handler
--FILE--
<?php
set_error_handler(function($code, $msg) {
    echo "Err: $msg\n";
    $GLOBALS['a']=9;
});
$a=[];
($a[PHP_INT_MAX+1]);
?>
DONE
--EXPECTF--
Err: The float %f is not representable as an int, cast occurred
Err: Undefined array key %i
DONE
