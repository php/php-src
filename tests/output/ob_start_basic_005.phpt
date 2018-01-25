--TEST--
ob_start(): non-static method as static callbacks.
--FILE--
<?php
/*
 * proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
 * Function is implemented in main/output.c
*/

Class C {
	function h($string) {
		return $string;
	}
}

function checkAndClean() {
  print_r(ob_list_handlers());
  while (ob_get_level()>0) {
    ob_end_flush();
  }
}

var_dump(ob_start('C::h'));
checkAndClean();

?>
--EXPECTF--
Warning: ob_start(): non-static method C::h() should not be called statically in %s on line 20
bool(true)
Array
(
    [0] => C::h
)
