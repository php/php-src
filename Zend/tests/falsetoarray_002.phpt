--TEST--
Autovivification of false to array with data clobbering by error handler
--FILE--
<?php
set_error_handler(function($code, $msg) {
    echo "Err: $msg\n";
    $GLOBALS['a']='';
});
$a=[!'a'];
$a[0][$d]='b';
var_dump($a);
?>
--EXPECT--
Err: Automatic conversion of false to array is deprecated
string(0) ""
