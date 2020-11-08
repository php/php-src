--TEST--
SPL: ArrayObject/Iterator from IteratorAggregate
--FILE--
<?php

// This test also needs to exclude the protected and private variables
// since they cannot be accessed from the external object which iterates
// them.

class test implements IteratorAggregate
{
    public    $pub = "public";
    protected $pro = "protected";
    private   $pri = "private";

    function __construct()
    {
        $this->imp = "implicit";
    }

    function getIterator()
    {
        $it = new ArrayObject($this);
        return $it->getIterator();
    }
};

$test = new test;
$test->dyn = "dynamic";

print_r($test);

print_r($test->getIterator());

foreach($test as $key => $val)
{
    echo "$key => $val\n";
}

?>
--EXPECT--
test Object
(
    [pub] => public
    [pro:protected] => protected
    [pri:test:private] => private
    [imp] => implicit
    [dyn] => dynamic
)
ArrayIterator Object
(
    [storage:ArrayIterator:private] => ArrayObject Object
        (
            [storage:ArrayObject:private] => test Object
                (
                    [pub] => public
                    [pro:protected] => protected
                    [pri:test:private] => private
                    [imp] => implicit
                    [dyn] => dynamic
                )

        )

)
pub => public
imp => implicit
dyn => dynamic
