--TEST--
ZE2 ArrayAccess
--FILE--
<?php
class ObjectOne implements ArrayAccess {

    public $a = array('1st', 1, 2=>'3rd', '4th'=>4);

    function offsetExists($index) {
        echo __METHOD__ . "($index)\n";
        return array_key_exists($index, $this->a);
    }
    function offsetGet($index) {
        echo __METHOD__ . "($index)\n";
        return $this->a[$index];
    }
    function offsetSet($index, $newval) {
        echo __METHOD__ . "($index,$newval)\n";
        return $this->a[$index] = $newval;
    }
    function offsetUnset($index) {
        echo __METHOD__ . "($index)\n";
        unset($this->a[$index]);
    }
}

$obj = new ObjectOne;

var_dump($obj->a);

echo "===EMPTY===\n";
var_dump(empty($obj[0]));
var_dump(empty($obj[1]));
var_dump(empty($obj[2]));
var_dump(empty($obj['4th']));
var_dump(empty($obj['5th']));
var_dump(empty($obj[6]));

echo "===isset===\n";
var_dump(isset($obj[0]));
var_dump(isset($obj[1]));
var_dump(isset($obj[2]));
var_dump(isset($obj['4th']));
var_dump(isset($obj['5th']));
var_dump(isset($obj[6]));

echo "===offsetGet===\n";
var_dump($obj[0]);
var_dump($obj[1]);
var_dump($obj[2]);
var_dump($obj['4th']);
var_dump($obj['5th']);
var_dump($obj[6]);

echo "===offsetSet===\n";
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
===EMPTY===
ObjectOne::offsetExists(0)
ObjectOne::offsetGet(0)
bool(false)
ObjectOne::offsetExists(1)
ObjectOne::offsetGet(1)
bool(false)
ObjectOne::offsetExists(2)
ObjectOne::offsetGet(2)
bool(false)
ObjectOne::offsetExists(4th)
ObjectOne::offsetGet(4th)
bool(false)
ObjectOne::offsetExists(5th)
bool(true)
ObjectOne::offsetExists(6)
bool(true)
===isset===
ObjectOne::offsetExists(0)
bool(true)
ObjectOne::offsetExists(1)
bool(true)
ObjectOne::offsetExists(2)
bool(true)
ObjectOne::offsetExists(4th)
bool(true)
ObjectOne::offsetExists(5th)
bool(false)
ObjectOne::offsetExists(6)
bool(false)
===offsetGet===
ObjectOne::offsetGet(0)
string(3) "1st"
ObjectOne::offsetGet(1)
int(1)
ObjectOne::offsetGet(2)
string(3) "3rd"
ObjectOne::offsetGet(4th)
int(4)
ObjectOne::offsetGet(5th)

Warning: Undefined array key "5th" in %s on line %d
NULL
ObjectOne::offsetGet(6)

Warning: Undefined array key 6 in %s on line %d
NULL
===offsetSet===
WRITE 1
ObjectOne::offsetSet(1,Changed 1)
ObjectOne::offsetGet(1)
string(9) "Changed 1"
WRITE 2
ObjectOne::offsetSet(4th,Changed 4th)
ObjectOne::offsetGet(4th)
string(11) "Changed 4th"
WRITE 3
ObjectOne::offsetSet(5th,Added 5th)
ObjectOne::offsetGet(5th)
string(9) "Added 5th"
WRITE 4
ObjectOne::offsetSet(6,Added 6)
ObjectOne::offsetGet(6)
string(7) "Added 6"
ObjectOne::offsetGet(0)
string(3) "1st"
ObjectOne::offsetGet(2)
string(3) "3rd"
ObjectOne::offsetSet(6,changed 6)
ObjectOne::offsetGet(6)
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
ObjectOne::offsetUnset(2)
ObjectOne::offsetUnset(4th)
ObjectOne::offsetUnset(7)
ObjectOne::offsetUnset(8th)
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
