--TEST--
GH-10248 (Assertion `!(zval_get_type(&(*(property))) == 10)' failed.)
--FILE--
<?php

class a extends ArrayIterator
{
    public ?int $property;
}
$a = new a();
$a->property = &$b;
$a->property;

echo "Done\n";

?>
--EXPECT--
Done
