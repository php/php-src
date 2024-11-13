--TEST--
Error handler can change type of operand of --
--FILE--
<?php

set_error_handler(function () {
    global $x;
    $x = 1;
});

$x = '';
$x--;
var_dump($x);

?>
DONE
--EXPECT--
int(-1)
DONE
