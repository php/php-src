--TEST--
Bug #74058 (ArrayObject can not notice changes)
--FILE--
<?php

class MyArrayObject extends ArrayObject
{
    public function __construct($input = [])
    {
        parent::__construct($input, ArrayObject::ARRAY_AS_PROPS);
    }

    public function offsetSet($x, $v)
    {
        echo "offsetSet('{$x}')\n";
        return parent::offsetSet($x, $v);
    }

    public function offsetGet($x)
    {
        echo "offsetGet('{$x}')\n";
        return parent::offsetGet($x);
    }
}

class MyArray extends ArrayObject
{
    public function __construct($input = [])
    {
        parent::__construct($input);
    }

    public function offsetSet($x, $v)
    {
        echo "offsetSet('{$x}')\n";
        return parent::offsetSet($x, $v);
    }

    public function offsetGet($x)
    {
        echo "offsetGet('{$x}')\n";
        return parent::offsetGet($x);
    }
}

$x = new MyArrayObject;
$x->a1 = new stdClass();
var_dump($x->a1);

$x->a1->b = 'some value';

$y = new MyArray();
$y['a2'] = new stdClass();
var_dump($y['a2']);

$y['a2']->b = 'some value';

?>
--EXPECTF--
offsetSet('a1')
offsetGet('a1')
object(stdClass)#%s (0) {
}
offsetGet('a1')
offsetSet('a2')
offsetGet('a2')
object(stdClass)#%s (0) {
}
offsetGet('a2')
