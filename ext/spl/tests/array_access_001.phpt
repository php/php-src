--TEST--
SPL: array_access
--SKIPIF--
<?php
	if (!extension_loaded("spl")) die("skip");
	if (!in_array("ArrayAccess", spl_classes())) die("skip spl_array_access not present");
?>
--FILE--
<?php
class c implements ArrayAccess {

	public $a = array('1st', 1, 2=>'3rd', '4th'=>4);
	function exists($index) {
		echo __METHOD__ . "($index)\n";
		return array_key_exists($index, $this->a);
	}
	function get($index) {
		echo __METHOD__ . "($index)\n";
		return $this->a[$index];
	}
	function set($index, $newval) {
		echo __METHOD__ . "($index,$newval)\n";
		return $this->a[$index] = $newval;
	}
	function del($index) {
		echo __METHOD__ . "($index)\n";
		unset($this->a[$index]);
	}
}

$obj = new c();

var_dump($obj->a);

var_dump($obj[0]);
var_dump($obj[1]);
var_dump($obj[2]);
var_dump($obj['4th']);
var_dump($obj['5th']);
var_dump($obj[6]);

echo "WRITE 1\n";
$obj[1] = 'Changed 1';
var_dump($obj[1]);
echo "WRITE 2\n";
$obj['4th'] = 'Changed 4th';
var_dump($obj['4th']);
echo "WRITE 3\n";
$obj['5th'] = 'Added 5th';
var_dump($obj['5th']);
echo "WRITE 4\n";
$obj[6] = 'Added 6';
var_dump($obj[6]);

var_dump($obj[0]);
var_dump($obj[2]);

$x = $obj[6] = 'changed 6';
var_dump($obj[6]);
var_dump($x);

echo "===unset===\n";
var_dump($obj->a);
unset($obj[2]);
unset($obj['4th']);
unset($obj[7]);
unset($obj['8th']);
var_dump($obj->a);

print "Done\n";
?>
--EXPECTF--
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
c::exists(0)
c::get(0)
string(3) "1st"
c::exists(1)
c::get(1)
int(1)
c::exists(2)
c::get(2)
string(3) "3rd"
c::exists(4th)
c::get(4th)
int(4)
c::exists(5th)

Notice: Undefined index:  5th in %sarray_access_001.php on line %d
NULL
c::exists(6)

Notice: Undefined index:  6 in %sarray_access_001.php on line %d
NULL
WRITE 1
c::set(1,Changed 1)
c::exists(1)
c::get(1)
string(9) "Changed 1"
WRITE 2
c::set(4th,Changed 4th)
c::exists(4th)
c::get(4th)
string(11) "Changed 4th"
WRITE 3
c::set(5th,Added 5th)
c::exists(5th)
c::get(5th)
string(9) "Added 5th"
WRITE 4
c::set(6,Added 6)
c::exists(6)
c::get(6)
string(7) "Added 6"
c::exists(0)
c::get(0)
string(3) "1st"
c::exists(2)
c::get(2)
string(3) "3rd"
c::set(6,changed 6)
c::exists(6)
c::get(6)
string(9) "changed 6"
string(9) "changed 6"
===unset===
array(6) {
  [0]=>
  string(3) "1st"
  [1]=>
  string(9) "Changed 1"
  [2]=>
  string(3) "3rd"
  ["4th"]=>
  string(11) "Changed 4th"
  ["5th"]=>
  string(9) "Added 5th"
  [6]=>
  string(9) "changed 6"
}
c::del(2)
c::del(4th)
c::del(7)
c::del(8th)
array(4) {
  [0]=>
  string(3) "1st"
  [1]=>
  string(9) "Changed 1"
  ["5th"]=>
  string(9) "Added 5th"
  [6]=>
  string(9) "changed 6"
}
Done
