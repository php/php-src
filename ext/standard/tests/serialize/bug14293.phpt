--TEST--
Bug #14293 (serialize() and __sleep())
--FILE--
<?php
class t
{
    function __construct()
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

Warning: serialize(): "b" returned as member variable from __sleep() but does not exist in %s on line %d
O:1:"t":1:{s:1:"a";s:5:"hello";}
object(t)#%d (1) {
  ["a"]=>
  string(5) "hello"
}
