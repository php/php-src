--TEST--
Bug #70730 (Incorrect ArrayObject serialization if unset is called in serialize())
--FILE--
<?php
class A extends \ArrayObject
{
    protected $foo;

    public function __construct()
    {
        $this->foo = 'bar';
    }

    public function serialize(): string
    {
        unset($this->foo);
        $result = parent::serialize();
        $this->foo = 'bar';
        return $result;
    }
}

$a = new A();
$a->append('item1');
$a->append('item2');
$a->append('item3');
$b = new A();
$b->unserialize($a->serialize());
var_dump($b);
?>
--EXPECTF--
object(A)#%d (2) {
  ["foo":protected]=>
  string(3) "bar"
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    string(5) "item1"
    [1]=>
    string(5) "item2"
    [2]=>
    string(5) "item3"
  }
}
