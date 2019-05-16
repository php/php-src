--TEST--
serialize()/unserialize()/var_dump()
--INI--
serialize_precision=100
--FILE--
<?php
class t
{
	function __construct()
	{
		$this->a = "hallo";
	}
}

class s
{
	public $a;
	public $b;
	public $c;

	function __construct()
	{
		$this->a = "hallo";
		$this->b = "php";
		$this->c = "world";
		$this->d = "!";
	}

	function __sleep()
	{
		echo "__sleep called\n";
		return array("a","c");
	}

	function __wakeup()
	{
		echo "__wakeup called\n";
	}
}


echo serialize(NULL)."\n";
echo serialize((bool) true)."\n";
echo serialize((bool) false)."\n";
echo serialize(1)."\n";
echo serialize(0)."\n";
echo serialize(-1)."\n";
echo serialize(2147483647)."\n";
echo serialize(-2147483647)."\n";
echo serialize(1.123456789)."\n";
echo serialize(1.0)."\n";
echo serialize(0.0)."\n";
echo serialize(-1.0)."\n";
echo serialize(-1.123456789)."\n";
echo serialize("hallo")."\n";
echo serialize(array(1,1.1,"hallo",NULL,true,array()))."\n";

$t = new t();
$data = serialize($t);
echo "$data\n";
$t = unserialize($data);
var_dump($t);

$t = new s();
$data = serialize($t);
echo "$data\n";
$t = unserialize($data);
var_dump($t);

$a = array("a" => "test");
$a[ "b" ] = &$a[ "a" ];
var_dump($a);
$data = serialize($a);
echo "$data\n";
$a = unserialize($data);
var_dump($a);
?>
--EXPECTF--
N;
b:1;
b:0;
i:1;
i:0;
i:-1;
i:2147483647;
i:-2147483647;
d:1.123456789000000011213842299184761941432952880859375;
d:1;
d:0;
d:-1;
d:-1.123456789000000011213842299184761941432952880859375;
s:5:"hallo";
a:6:{i:0;i:1;i:1;d:1.100000000000000088817841970012523233890533447265625;i:2;s:5:"hallo";i:3;N;i:4;b:1;i:5;a:0:{}}
O:1:"t":1:{s:1:"a";s:5:"hallo";}
object(t)#%d (1) {
  ["a"]=>
  string(5) "hallo"
}
__sleep called
O:1:"s":2:{s:1:"a";s:5:"hallo";s:1:"c";s:5:"world";}
__wakeup called
object(s)#%d (3) {
  ["a"]=>
  string(5) "hallo"
  ["b"]=>
  NULL
  ["c"]=>
  string(5) "world"
}
array(2) {
  ["a"]=>
  &string(4) "test"
  ["b"]=>
  &string(4) "test"
}
a:2:{s:1:"a";s:4:"test";s:1:"b";R:2;}
array(2) {
  ["a"]=>
  &string(4) "test"
  ["b"]=>
  &string(4) "test"
}
