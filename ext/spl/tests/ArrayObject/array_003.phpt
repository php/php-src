--TEST--
SPL: ArrayObject from object
--FILE--
<?php

// This test also needs to exclude the protected and private variables
// since they cannot be accessed from the external object which iterates
// them.

#[AllowDynamicProperties]
class test
{
    public    $pub = "public";
    protected $pro = "protected";
    private   $pri = "private";

    function __construct()
    {
        $this->imp = "implicit";
    }
};

$test = new test;
$test->dyn = "dynamic";

print_r($test);

$object = new ArrayObject($test);

print_r($object);

foreach($test as $key => $val)
{
    echo "$key => $val\n";
}

?>
--EXPECTF--
test Object
(
    [pub] => public
    [pro:protected] => protected
    [pri:test:private] => private
    [imp] => implicit
    [dyn] => dynamic
)

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
ArrayObject Object
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
pub => public
imp => implicit
dyn => dynamic
