--TEST--
Capturing array in user error handler during index conversion
--FILE--
<?php
set_error_handler(function($code, $msg) {
    echo "Err: $msg\n";
    $GLOBALS[''] = $GLOBALS['y'];
});
function x(&$s){
    $s[100000000000000000000] = 1;
}
x($y);
var_dump($y);
?>
--EXPECT--
Err: non-representable float 1.0E+20 was cast to int
array(0) {
}
