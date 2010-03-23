--TEST--
ob_start(): multiple buffer initialization with a single call, using arrays.
--FILE--
<?php
/* 
 * proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
 * Function is implemented in main/output.c
*/ 

function f($string) {
	static $i=0;
	$i++;
	$len = strlen($string);
	return "f[call:$i; len:$len] - $string\n";
}

Class C {
	public $id = 'none';

	function __construct($id) {
		$this->id = $id;
	}

	static function g($string) {
		static $i=0;
		$i++;
		$len = strlen($string);
		return "C::g[call:$i; len:$len] - $string\n";
	}
	
	function h($string) {
		static $i=0;
		$i++;
		$len = strlen($string);
		return "C::h[call:$i; len:$len; id:$this->id] - $string\n";
	}
}

function checkAndClean() {
  print_r(ob_list_handlers());
  while (ob_get_level()>0) {
    ob_end_flush();
  }
}

echo "\n ---> Test arrays: \n";
var_dump(ob_start(array("f")));
checkAndClean();

var_dump(ob_start(array("f", "f")));
checkAndClean();

var_dump(ob_start(array("f", "C::g", "f", "C::g")));
checkAndClean();

var_dump(ob_start(array("f", "non_existent", "f")));
checkAndClean();

var_dump(ob_start(array("f", "non_existent", "f", "f")));
checkAndClean();

$c = new c('originalID');
var_dump(ob_start(array($c, "h")));
checkAndClean();

var_dump(ob_start(array($c, "h")));
$c->id = 'changedID';
checkAndClean();

$c->id = 'changedIDagain';
var_dump(ob_start(array('f', 'C::g', array(array($c, "g"), array($c, "h")))));
checkAndClean();
?>
--EXPECTF--

 ---> Test arrays: 
f[call:1; len:34] - bool(true)
Array
(
    [0] => f
)

f[call:3; len:68] - f[call:2; len:47] - bool(true)
Array
(
    [0] => f
    [1] => f
)


f[call:5; len:150] - C::g[call:2; len:125] - f[call:4; len:103] - C::g[call:1; len:79] - bool(true)
Array
(
    [0] => f
    [1] => C::g
    [2] => f
    [3] => C::g
)




f[call:6; len:35] - bool(false)
Array
(
    [0] => f
)

f[call:7; len:35] - bool(false)
Array
(
    [0] => f
)

C::h[call:1; len:37; id:originalID] - bool(true)
Array
(
    [0] => C::h
)

C::h[call:2; len:37; id:changedID] - bool(true)
Array
(
    [0] => C::h
)

f[call:8; len:175] - C::g[call:4; len:150] - C::g[call:3; len:125] - C::h[call:3; len:82; id:changedIDagain] - bool(true)
Array
(
    [0] => f
    [1] => C::g
    [2] => C::g
    [3] => C::h
)