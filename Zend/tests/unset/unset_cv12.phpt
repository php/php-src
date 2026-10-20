--TEST--
unset() CV 12 (unset() in indirect called function)
--FILE--
<?php
$x = 1;
function foo() {unset($GLOBALS["x"]);}
call_user_func("foo");
echo "ok\n";
?>
--EXPECT--
ok
