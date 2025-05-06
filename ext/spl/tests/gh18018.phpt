--TEST--
GH-18018 (RC1 data returned from offsetGet causes UAF in ArrayObject)
--FILE--
<?php
class Crap extends ArrayObject
{
    public function offsetGet($offset): mixed
    {
        return [random_int(1,1)];
    }
}

$values = ['qux' => 1];

$object = new Crap($values);

var_dump(empty($object['qux']));
?>
--EXPECT--
bool(false)
