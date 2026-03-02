--TEST--
Bug #76773 (Traits used on the parent are ignored for child classes)
--FILE--
<?php

trait MyTrait
{
    public function hello()
    {
        echo __CLASS__, "\n";

        if (get_parent_class(__CLASS__) !== false) {
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
--EXPECT--
ChildClass
ParentClass
