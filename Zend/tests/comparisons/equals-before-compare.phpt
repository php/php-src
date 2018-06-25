--TEST--
__equals: Should not be called for comparisons.
--FILE--
<?php
class A
{
    public function __equals($other)
    {
        return 0;
    }

    public function __compareTo($other)
    {
        return 1;
    }
}

$a = new A();

var_dump($a <=> 1);   
var_dump($a <=> $a);

?>
--EXPECTF--
int(1)
int(1)
