--TEST--
Bug #77291 (magic methods inherited from a trait may be ignored)
--FILE--
<?php

trait AccessibleProperties
{
    public function __isset($property)
    {
        return property_exists($this, $property);
    }

    public function __get($property)
    {
        if (property_exists($this, $property)) {
            return $this->$property;
        }
    }
}

class Foo4567 {
    use AccessibleProperties;

    protected $a = 'Some value';
}

class Foo45 {
    use AccessibleProperties;

    protected $a = 'Some value';
}

$foo = new Foo4567;
var_dump(isset($foo->a));
$foo = new Foo45;
var_dump($foo->a);
?>
===DONE===
--EXPECT--
bool(true)
string(10) "Some value"
===DONE===
