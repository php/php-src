--TEST--
Bug #14293 (serialize() and __sleep())
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
--EXPECTF--
__sleep called

Notice: serialize(): "b" returned as member variable from __sleep() but does not exist in %sbug14293.php on line %d
O:1:"t":2:{s:1:"a";s:5:"hello";s:1:"b";N;}
object(t)#%d (2) {
  ["a"]=>
  string(5) "hello"
  ["b"]=>
  NULL
}
