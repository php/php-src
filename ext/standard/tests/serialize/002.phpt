--TEST--
serialize() (Bug #14293)
--POST--
--GET--
--FILE--
<?php 
class t
{
	function t()
	{
		$this->a = 'hello';
	}

	function __sleep()
	{
		echo "__sleep called\n";
		return array('a','b');
	}	
}

$t = new t();
$data = serialize($t);
echo "$data\n";
$t = unserialize($data);
var_dump($t);

?>
--EXPECT--
__sleep called
O:1:"t":1:{s:1:"a";s:5:"hello";}
object(t)(1) {
  ["a"]=>
  string(5) "hello"
}
