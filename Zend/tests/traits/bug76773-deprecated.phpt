--TEST--
Bug #76773 (Traits used on the parent are ignored for child classes) [original test with deprecated syntax]
--FILE--
<?php

trait MyTrait
{
    public function hello()
    {
        echo __CLASS__, "\n";

        if (\is_callable(array('parent', __FUNCTION__))) {
            parent::hello();
        }
    }
}

class ParentClass
{
    use MyTrait;
}

class ChildClass extends ParentClass
{
    use MyTrait;
}

$c = new ChildClass();
$c->hello();
?>
--EXPECTF--
ChildClass

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
ParentClass
