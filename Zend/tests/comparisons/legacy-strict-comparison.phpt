--TEST--
__compareTo: Check that === isn't affected by __compareTo
--FILE--
<?php

class Test
{
    public function __compareTo($other)
    {
        return 1;
    }
}

$a = new Test();
$b = new Test();

var_dump($a === $a);
var_dump($a === $b);

?>
--EXPECT--
bool(true)
bool(false)
