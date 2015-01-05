--TEST--
Bug #54367 (Use of closure causes problem in ArrayAccess)
--FILE--
<?php
class MyObjet implements ArrayAccess
{
    public function offsetSet($offset, $value) { }
    public function offsetExists($offset) {  }
    public function offsetUnset($offset) { }

    public function offsetGet ($offset)
    {
	return function ($var) use ($offset) { // here is the problem
              var_dump($offset, $var);
        };
    }
}

$a = new MyObjet();
echo $a['p']('foo');
?>
--EXPECT--
string(1) "p"
string(3) "foo"
