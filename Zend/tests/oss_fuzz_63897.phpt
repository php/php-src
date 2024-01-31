--TEST--
oss-fuzz #63897: ++/-- leaks reference created in undefined variable handler
--FILE--
<?php
set_error_handler(function () {
    global $x;
});
$x--;
unset($x);
$x++;
unset($x);
--$x;
unset($x);
++$x;
?>
===DONE===
--EXPECT--
===DONE===
