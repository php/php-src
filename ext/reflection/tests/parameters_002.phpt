--TEST--
ReflectionParameter::getclass(), getDeclaringClass(), getDeclaringFunction()
--FILE--
<?php

function test($nix, Array $ar, &$ref, stdClass $std, NonExistingClass $na, stdClass &$opt = NULL, $def = "FooBar")
{
}

class test
{
	function test($nix, Array $ar, &$ref, stdClass $std, NonExistingClass $na, stdClass $opt = NULL, $def = "FooBar")
	{
	}
}

function check_params_decl_func($r, $f)
{
	$c = $r->$f();
	echo $f . ': ' . ($c ? ($c instanceof ReflectionMethod ? $c->class . '::' : '') . $c->name : 'NULL') . "()\n";
}

function check_params_decl_class($r, $f)
{
	$c = $r->$f();
	echo $f . ': ' . ($c ? $c->name : 'NULL') . "\n";
}

function check_params_func($r, $f)
{
	echo $f . ': ';
	$v = $r->$f();
	var_dump($v);
}

function check_params($r)
{
	echo "#####" . ($r instanceof ReflectionMethod ? $r->class . '::' : '') . $r->name . "()#####\n";
	foreach($r->getParameters() as $p)
	{
		echo "===" . $p->getPosition() . "===\n";
		check_params_func($p, 'getName');
		check_params_func($p, 'isPassedByReference');
		try
		{
			check_params_decl_class($p, 'getClass');
		}
		catch(ReflectionException $e)
		{
			echo $e->getMessage() . "\n";
		}
		check_params_decl_class($p, 'getDeclaringClass');
		check_params_decl_func($p, 'getDeclaringFunction');
		check_params_func($p, 'isArray');
		check_params_func($p, 'allowsNull');
		check_params_func($p, 'isOptional');
		check_params_func($p, 'isDefaultValueAvailable');
		if ($p->isOptional())
		{
			check_params_func($p, 'getDefaultValue');
		}
	}
}

check_params(new ReflectionFunction('test'));

check_params(new ReflectionMethod('test::test'));

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
#####test()#####
===0===
getName: unicode(3) "nix"
isPassedByReference: bool(false)
getClass: NULL
getDeclaringClass: NULL
getDeclaringFunction: test()
isArray: bool(false)
allowsNull: bool(true)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===1===
getName: unicode(2) "ar"
isPassedByReference: bool(false)
getClass: NULL
getDeclaringClass: NULL
getDeclaringFunction: test()
isArray: bool(true)
allowsNull: bool(false)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===2===
getName: unicode(3) "ref"
isPassedByReference: bool(true)
getClass: NULL
getDeclaringClass: NULL
getDeclaringFunction: test()
isArray: bool(false)
allowsNull: bool(true)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===3===
getName: unicode(3) "std"
isPassedByReference: bool(false)
getClass: stdClass
getDeclaringClass: NULL
getDeclaringFunction: test()
isArray: bool(false)
allowsNull: bool(false)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===4===
getName: unicode(2) "na"
isPassedByReference: bool(false)
Class NonExistingClass does not exist
getDeclaringClass: NULL
getDeclaringFunction: test()
isArray: bool(false)
allowsNull: bool(false)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===5===
getName: unicode(3) "opt"
isPassedByReference: bool(true)
getClass: stdClass
getDeclaringClass: NULL
getDeclaringFunction: test()
isArray: bool(false)
allowsNull: bool(true)
isOptional: bool(true)
isDefaultValueAvailable: bool(true)
getDefaultValue: NULL
===6===
getName: unicode(3) "def"
isPassedByReference: bool(false)
getClass: NULL
getDeclaringClass: NULL
getDeclaringFunction: test()
isArray: bool(false)
allowsNull: bool(true)
isOptional: bool(true)
isDefaultValueAvailable: bool(true)
getDefaultValue: unicode(6) "FooBar"
#####test::test()#####
===0===
getName: unicode(3) "nix"
isPassedByReference: bool(false)
getClass: NULL
getDeclaringClass: test
getDeclaringFunction: test::test()
isArray: bool(false)
allowsNull: bool(true)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===1===
getName: unicode(2) "ar"
isPassedByReference: bool(false)
getClass: NULL
getDeclaringClass: test
getDeclaringFunction: test::test()
isArray: bool(true)
allowsNull: bool(false)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===2===
getName: unicode(3) "ref"
isPassedByReference: bool(true)
getClass: NULL
getDeclaringClass: test
getDeclaringFunction: test::test()
isArray: bool(false)
allowsNull: bool(true)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===3===
getName: unicode(3) "std"
isPassedByReference: bool(false)
getClass: stdClass
getDeclaringClass: test
getDeclaringFunction: test::test()
isArray: bool(false)
allowsNull: bool(false)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===4===
getName: unicode(2) "na"
isPassedByReference: bool(false)
Class NonExistingClass does not exist
getDeclaringClass: test
getDeclaringFunction: test::test()
isArray: bool(false)
allowsNull: bool(false)
isOptional: bool(false)
isDefaultValueAvailable: bool(false)
===5===
getName: unicode(3) "opt"
isPassedByReference: bool(false)
getClass: stdClass
getDeclaringClass: test
getDeclaringFunction: test::test()
isArray: bool(false)
allowsNull: bool(true)
isOptional: bool(true)
isDefaultValueAvailable: bool(true)
getDefaultValue: NULL
===6===
getName: unicode(3) "def"
isPassedByReference: bool(false)
getClass: NULL
getDeclaringClass: test
getDeclaringFunction: test::test()
isArray: bool(false)
allowsNull: bool(true)
isOptional: bool(true)
isDefaultValueAvailable: bool(true)
getDefaultValue: unicode(6) "FooBar"
===DONE===
