--TEST--
SPL: array_read
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

echo "EXTERNAL\n";

$a = array('1st', 1, 2=>'3rd', '4th'=>4);
var_dump($a);

class external implements spl_array_read {

	function exists($index) {
		echo __METHOD__ . "($index)\n";
		return array_key_exists($index, $GLOBALS['a']);
	}

	function get($index) {
		echo __METHOD__ . "($index)\n";
		return $GLOBALS['a'][$index];
	}
}

$obj = new external();

var_dump($obj->get(0));
var_dump($obj->get(1));
var_dump($obj->get(2));
var_dump($obj->get('4th'));
var_dump($obj->get('5th'));
var_dump($obj->get(6));

var_dump($obj[0]);
var_dump($obj[1]);
var_dump($obj[2]);
var_dump($obj['4th']);
var_dump($obj['5th']);
var_dump($obj[6]);

$out = $obj[0]; echo "$out\n";
$out = $obj[1]; echo "$out\n";
$out = $obj[2]; echo "$out\n";
$out = $obj['4th']; echo "$out\n";

echo "INTERNAL\n";

class internal implements spl_array_read {

	public $a = array('1st', 1, 2=>'3rd', '4th'=>4);

	function exists($index) {
		echo __METHOD__ . "($index)\n";
		return array_key_exists($index, $GLOBALS['a']);
	}

	function get($index) {
		echo __METHOD__ . "($index)\n";
		return $GLOBALS['a'][$index];
	}
}

$obj = new internal();

var_dump($obj->a);

var_dump($obj->get(0));
var_dump($obj->get(1));
var_dump($obj->get(2));
var_dump($obj->get('4th'));
var_dump($obj->get('5th'));
var_dump($obj->get(6));

var_dump($obj[0]);
var_dump($obj[1]);
var_dump($obj[2]);
var_dump($obj['4th']);
var_dump($obj['5th']);
var_dump($obj[6]);

$out = $obj[0]; echo "$out\n";
$out = $obj[1]; echo "$out\n";
$out = $obj[2]; echo "$out\n";
$out = $obj['4th']; echo "$out\n";

print "Done\n";
?>
--EXPECTF--
EXTERNAL
array(4) {
  [0]=>
  string(3) "1st"
  [1]=>
  int(1)
  [2]=>
  string(3) "3rd"
  ["4th"]=>
  int(4)
}
external::get(0)
string(3) "1st"
external::get(1)
int(1)
external::get(2)
string(3) "3rd"
external::get(4th)
int(4)
external::get(5th)

Notice: Undefined index:  5th in %s on line %d
NULL
external::get(6)

Notice: Undefined offset:  6 in %s on line %d
NULL
external::exists(0)
external::get(0)
string(3) "1st"
external::exists(1)
external::get(1)
int(1)
external::exists(2)
external::get(2)
string(3) "3rd"
external::exists(4th)
external::get(4th)
int(4)
external::exists(5th)

Notice: Undefined index:  5th in %s on line %d
NULL
external::exists(6)

Notice: Undefined index:  6 in %s on line %d
NULL
external::exists(0)
external::get(0)
1st
external::exists(1)
external::get(1)
1
external::exists(2)
external::get(2)
3rd
external::exists(4th)
external::get(4th)
4
INTERNAL
array(4) {
  [0]=>
  string(3) "1st"
  [1]=>
  int(1)
  [2]=>
  string(3) "3rd"
  ["4th"]=>
  int(4)
}
internal::get(0)
string(3) "1st"
internal::get(1)
int(1)
internal::get(2)
string(3) "3rd"
internal::get(4th)
int(4)
internal::get(5th)

Notice: Undefined index:  5th in %s on line %d
NULL
internal::get(6)

Notice: Undefined offset:  6 in %s on line %d
NULL
internal::exists(0)
internal::get(0)
string(3) "1st"
internal::exists(1)
internal::get(1)
int(1)
internal::exists(2)
internal::get(2)
string(3) "3rd"
internal::exists(4th)
internal::get(4th)
int(4)
internal::exists(5th)

Notice: Undefined index:  5th in %s on line %d
NULL
internal::exists(6)

Notice: Undefined index:  6 in %s on line %d
NULL
internal::exists(0)
internal::get(0)
1st
internal::exists(1)
internal::get(1)
1
internal::exists(2)
internal::get(2)
3rd
internal::exists(4th)
internal::get(4th)
4
Done
