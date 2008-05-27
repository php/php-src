--TEST--
Reflection properties are read only
--FILE--
<?php

class ReflectionMethodEx extends ReflectionMethod
{
	public $foo = "xyz";
	
	function __construct($c,$m)
	{
		echo __METHOD__ . "\n";
		parent::__construct($c,$m);
	}
}

$r = new ReflectionMethodEx('ReflectionMethodEx','getName');

var_dump($r->class);
var_dump($r->name);
var_dump($r->foo);
@var_dump($r->bar);

try
{
	$r->class = 'bullshit';
}
catch(ReflectionException $e)
{
	echo $e->getMessage() . "\n";
}
try
{
$r->name = 'bullshit';
}
catch(ReflectionException $e)
{
	echo $e->getMessage() . "\n";
}

$r->foo = 'bar';
$r->bar = 'baz';

var_dump($r->class);
var_dump($r->name);
var_dump($r->foo);
var_dump($r->bar);

?>
===DONE===
--EXPECTF--
ReflectionMethodEx::__construct
unicode(18) "ReflectionMethodEx"
unicode(7) "getName"
unicode(3) "xyz"
NULL
Cannot set read-only property ReflectionMethodEx::$class
Cannot set read-only property ReflectionMethodEx::$name
unicode(18) "ReflectionMethodEx"
unicode(7) "getName"
unicode(3) "bar"
unicode(3) "baz"
===DONE===
