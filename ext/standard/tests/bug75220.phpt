--TEST--
Bug #75220 (is_callable crash for 'parent')
--FILE--
<?php

$a = new A();
$a->bar('foo');

class B {};
class A extends B
{
    function bar($func)
    {
        var_dump('foo');
        var_dump(is_callable('parent::foo'));
        var_dump(is_callable(array('parent', 'foo')));
    }

    function __call($func, $args)
    {
    }
};

?>
--EXPECT--
string(3) "foo"
bool(false)
bool(false)
