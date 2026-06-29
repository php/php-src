--TEST--
string offset 008 indirect string modification by error handler
--FILE--
<?php
set_error_handler(function($code, $msg) {
    echo "Err: $msg\n";
    $GLOBALS['a']=8;
});
$z = "z";
$a=["xx$z"];
var_dump($a[0][$b]);
var_dump($a);
?>
--EXPECT--
string(1) "x"
Err: Undefined variable $b
Err: String offset cast occurred
int(8)
