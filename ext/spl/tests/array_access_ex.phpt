--TEST--
SPL: array_access
--SKIPIF--
<?php
	if (!extension_loaded("spl")) die("skip");
	if (!in_array("spl::array_access",spl_classes())) die("skip spl::array_access not present");
?>
--FILE--
<?php
class array_write implements spl::array_writer {
	private $obj;
	private $idx;

	function __construct(&$obj, $index = null) {
		$this->obj = &$obj;
		$this->idx = $index;
	}

	function set($value) {
		echo __METHOD__ . "($value,".$this->idx.")\n";
		return $this->obj->set($this->idx, $value);
	}
}

class c implements spl::array_access_ex {

	public $a = array('1st', 1, 2=>'3rd', '4th'=>4);

	function new_writer($index) {
		return new array_write(&$this, $index);
	}

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

Notice: Undefined index:  5th in /usr/src/php5/ext/spl/tests/array_access_ex.php on line 49
NULL
c::exists(6)

Notice: Undefined index:  6 in /usr/src/php5/ext/spl/tests/array_access_ex.php on line 50
NULL
WRITE 1
c::exists(1)
array_write::set(Changed 1,1)
c::set(1,Changed 1)
c::exists(1)
c::get(1)
string(9) "Changed 1"
WRITE 2
c::exists(4th)
array_write::set(Changed 4th,4th)
c::set(4th,Changed 4th)
c::exists(4th)
c::get(4th)
string(11) "Changed 4th"
WRITE 3
c::exists(5th)
array_write::set(Added 5th,5th)
c::set(5th,Added 5th)
c::exists(5th)
c::get(5th)
string(9) "Added 5th"
WRITE 4
c::exists(6)
array_write::set(Added 6,6)
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
c::exists(6)
array_write::set(changed 6,6)
c::set(6,changed 6)
c::exists(6)
c::get(6)
string(9) "changed 6"
string(9) "changed 6"
Done
