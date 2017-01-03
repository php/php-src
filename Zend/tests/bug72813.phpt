--TEST--
Bug #72813 (Segfault with __get returned by ref)
--FILE--
<?php
class Test
{
    private $props = ['a' => 'text', 'b' => 1];

    public function &__get($prop)
    {
        return $this->props[$prop];
    }
    
    public function __set($prop, $value)
    {
        if ($prop === 'b') $value = [$value];
        $this->props[$prop] = $value;
    }
    
    public function getProperties()
    {
        return [$this->props];
    }
}

$obj = new Test;
$obj->b = $obj->b;
print_r($obj->getProperties());
?>
--EXPECT--
Array
(
    [0] => Array
        (
            [a] => text
            [b] => Array
                (
                    [0] => 1
                )

        )

)
