--TEST--
SPL: ArrayObject from object
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

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
ArrayObject Object
(
    [pub] => public
    [pro:protected] => protected
    [pri:private] => private
    [imp] => implicit
    [dyn] => dynamic
)
===DONE===
