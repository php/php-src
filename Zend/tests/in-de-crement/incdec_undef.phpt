--TEST--
Inc/dec undef var with error handler
--FILE--
<?php
set_error_handler(function($_, $m) {
    echo "$m\n";
    unset($GLOBALS['x']);
});
var_dump($x--);
unset($x);
var_dump($x++);
unset($x);
var_dump(--$x);
unset($x);
var_dump(++$x);
?>
--EXPECT--
Undefined variable $x
NULL
Undefined variable $x
NULL
Undefined variable $x
NULL
Undefined variable $x
int(1)
