--TEST--
Bug GH-10747 (Private fields in serialized DateInterval objects throw)
--FILE--
<?php
class I extends DateInterval
{
	private   int $var1;
	private       $var2 = 2;
	protected int $var3 = 3;
	protected     $var4;

	function __construct($tz)
	{
		parent::__construct($tz);
		$this->var1 = 1;
		$this->var4 = 4;
	}
}

$i = new I('P3D');
$s = serialize($i);
$u = unserialize($s);

var_dump($i, str_replace(chr(0), '!', $s), $u);
?>
--EXPECT--
object(I)#1 (15) {
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(3)
  ["h"]=>
  int(0)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
  ["from_string"]=>
  bool(false)
  ["date_string"]=>
  NULL
  ["var1":"I":private]=>
  int(1)
  ["var2":"I":private]=>
  int(2)
  ["var3":protected]=>
  int(3)
  ["var4":protected]=>
  int(4)
}
string(245) "O:1:"I":15:{s:1:"y";i:0;s:1:"m";i:0;s:1:"d";i:3;s:1:"h";i:0;s:1:"i";i:0;s:1:"s";i:0;s:1:"f";d:0;s:6:"invert";i:0;s:4:"days";b:0;s:11:"from_string";b:0;s:11:"date_string";N;s:7:"!I!var1";i:1;s:7:"!I!var2";i:2;s:7:"!*!var3";i:3;s:7:"!*!var4";i:4;}"
object(I)#2 (15) {
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(3)
  ["h"]=>
  int(0)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
  ["from_string"]=>
  bool(false)
  ["date_string"]=>
  NULL
  ["var1":"I":private]=>
  int(1)
  ["var2":"I":private]=>
  int(2)
  ["var3":protected]=>
  int(3)
  ["var4":protected]=>
  int(4)
}
