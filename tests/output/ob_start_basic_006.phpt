--TEST--
ob_start(): ensure multiple buffer initialization with a single call using arrays is not supported on PHP6 (http://bugs.php.net/42641)
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

Warning: ob_start(): array must have exactly two members in %s on line 44

Notice: ob_start(): failed to create buffer in %s on line 44
bool(false)
Array
(
)

Warning: ob_start(): class 'f' not found in %s on line 47

Notice: ob_start(): failed to create buffer in %s on line 47
bool(false)
Array
(
)

Warning: ob_start(): array must have exactly two members in %s on line 50

Notice: ob_start(): failed to create buffer in %s on line 50
bool(false)
Array
(
)

Warning: ob_start(): array must have exactly two members in %s on line 53

Notice: ob_start(): failed to create buffer in %s on line 53
bool(false)
Array
(
)

Warning: ob_start(): array must have exactly two members in %s on line 56

Notice: ob_start(): failed to create buffer in %s on line 56
bool(false)
Array
(
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


Warning: ob_start(): array must have exactly two members in %s on line 68

Notice: ob_start(): failed to create buffer in %s on line 68
bool(false)
Array
(
)
