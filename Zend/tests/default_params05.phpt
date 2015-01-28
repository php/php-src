--TEST--
Default parameters - 5
--FILE--
<?php
function test($a=1, $b=2, $c=3) {
	debug_print_backtrace();
}

test(7);
test(default,4,5);
test(default,default,8);
test(3,default,5);
echo "Done\n";
?>
--EXPECTF--	
#0  test(7) called at [%s:%d]
#0  test(1, 4, 5) called at [%s.php:%d]
#0  test(1, 2, 8) called at [%s:%d]
#0  test(3, 2, 5) called at [%s:%d]
Done
