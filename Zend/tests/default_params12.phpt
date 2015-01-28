--TEST--
Default parameters - 12, call_user_func
--FILE--
<?php
function foo($a, $b=1, $c=2, $d = 3) {
	var_dump($a, $b, $c, $d);
	var_dump(func_num_args());
	var_dump(func_get_args());
	echo "==\n";
}

call_user_func('foo', default, default,5,6);
echo "Done\n";
?>
--EXPECTF--	
Fatal error: Defaults can be used only for declared optional parameters in %s line %d