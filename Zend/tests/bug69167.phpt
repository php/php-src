--TEST--
Bug #69167 (call_user_func does not support references anymore)
--FILE--
<?php
function l($m) {
	    echo $m . "\n";
}

$cb = 'l';
call_user_func($cb, 'hi');

$cb2 = &$cb;
call_user_func($cb2, 'hi2');
?>
--EXPECT--
hi
hi2
