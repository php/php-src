--TEST--
SPL: ArrayObject/Iterator from IteratorAggregate
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
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
===DONE===
<?php exit(0); ?>
--EXPECTF--
test Object
(
    [pub] => public
    [pro:protected] => protected
    [pri:private] => private
    [imp] => implicit
    [dyn] => dynamic
)
ArrayIterator Object
(
    [pub] => public
    [pro:protected] => protected
    [pri:private] => private
    [imp] => implicit
    [dyn] => dynamic
)
pub => public
imp => implicit
dyn => dynamic
===DONE===
