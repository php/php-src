--TEST--
__equals: Equal objects automatically have the same ordering
--FILE--
<?php
class A
{
    public function __equals($other)
    {
        return $other % 2 == 0;
    }

    public function __compareTo($other)
    {
        return 1;
    }
}

var_dump(new A <=> 1);
var_dump(new A <=> 2);

?>
--EXPECTF--
int(1)
int(0)
