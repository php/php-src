--TEST--
Bug #64070 (Inheritance with Traits failed with error)
--FILE--
<?php
trait first_trait
{
    function first_function()
    {
        echo "From First Trait\n";
    }
}

trait second_trait
{
    use first_trait {
        first_trait::first_function as second_function;
    }

    function first_function()
    {
        echo "From Second Trait\n";
    }
}

class first_class
{
    use second_trait;
}

$obj = new first_class();
$obj->first_function();
$obj->second_function();
?>
--EXPECT--
From Second Trait
From First Trait
