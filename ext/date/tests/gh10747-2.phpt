--TEST--
Bug GH-10747 (Private fields in serialized DateTimeZone objects throw)
--FILE--
<?php
class I extends DateTimeZone
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

$i = new I('Europe/Kyiv');
$s = serialize($i);
$u = unserialize($s);

var_dump($i, str_replace(chr(0), '!', $s), $u);
?>
--EXPECTF--
object(I)#1 (6) {
  ["var1":"I":private]=>
  int(1)
  ["var2":"I":private]=>
  int(2)
  ["var3":protected]=>
  int(3)
  ["var4":protected]=>
  int(4)
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(11) "Europe/Kyiv"
}
string(143) "O:1:"I":6:{s:13:"timezone_type";i:3;s:8:"timezone";s:11:"Europe/Kyiv";s:7:"!I!var1";i:1;s:7:"!I!var2";i:2;s:7:"!*!var3";i:3;s:7:"!*!var4";i:4;}"
object(I)#2 (6) {
  ["var1":"I":private]=>
  int(1)
  ["var2":"I":private]=>
  int(2)
  ["var3":protected]=>
  int(3)
  ["var4":protected]=>
  int(4)
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(11) "Europe/Kyiv"
}
