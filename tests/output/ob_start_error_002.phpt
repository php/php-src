--TEST--
Test wrong number of arguments and wrong arg types for ob_start()
--FILE--
<?php
/* 
 * proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
 * Function is implemented in main/output.c
*/ 

Class C {
	static function f($str) {
		return $str;
	}
}

var_dump(ob_start(array("nonExistent","f")));
var_dump(ob_start(array("C","nonExistent")));
var_dump(ob_start("C::no"));
var_dump(ob_start("no"));
echo "done"
?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
bool(false)
done