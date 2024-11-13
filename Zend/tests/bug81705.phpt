--TEST--
Bug #81705 (type confusion/UAF on set_error_handler with concat operation)
--FILE--
<?php

$arr = [0];
$my_var = str_repeat("a", 1);
set_error_handler(
    function() use(&$my_var) {
        echo("error\n");
        $my_var = 0x123;
    }
);
$my_var .= $GLOBALS["arr"];
var_dump($my_var);
?>
--EXPECT--
error
string(6) "aArray"