--TEST--
SPL: array_access without return in set()
--SKIPIF--
<?php
	if (!extension_loaded("spl")) die("skip");
	if (!in_array("spl_array_access", spl_classes())) die("skip spl_array_access not present");
?>
--FILE--
<?php
class c implements spl_array_access {

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
		/* return */ $this->a[$index] = $newval;
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

Notice: Undefined index:  5th in %sarray_access_002.php on line %d
NULL
c::exists(6)

Notice: Undefined index:  6 in %sarray_access_002.php on line %d
NULL
WRITE 1
c::set(1,Changed 1)

Warning: Method c::set() did not return a value, using input value in %sarray_access_002.php on line %d
c::exists(1)
c::get(1)
string(9) "Changed 1"
WRITE 2
c::set(4th,Changed 4th)

Warning: Method c::set() did not return a value, using input value in %sarray_access_002.php on line %d
c::exists(4th)
c::get(4th)
string(11) "Changed 4th"
WRITE 3
c::set(5th,Added 5th)

Warning: Method c::set() did not return a value, using input value in %sarray_access_002.php on line %d
c::exists(5th)
c::get(5th)
string(9) "Added 5th"
WRITE 4
c::set(6,Added 6)

Warning: Method c::set() did not return a value, using input value in %sarray_access_002.php on line %d
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

Warning: Method c::set() did not return a value, using input value in %sarray_access_002.php on line %d
c::exists(6)
c::get(6)
string(9) "changed 6"
string(9) "changed 6"
Done
