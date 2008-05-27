--TEST--
ReflectionClass::[gs]etStaticPropertyValue
--FILE--
<?php

/* ReflectionClass cannot touch protected or private static properties */

/* ReflectionClass cannot create or delete static properties */

Class Test
{
	static public    $pub = 'pub';
	static protected $pro = 'pro';
	static private   $pri = 'pri';
	
	static function testing()
	{
		$ref = new ReflectionClass('Test');

		foreach(array('pub', 'pro', 'pri') as $name)
		{
			try
			{
				var_dump($ref->getStaticPropertyValue($name));
				var_dump($ref->getStaticPropertyValue($name));
				$ref->setStaticPropertyValue($name, 'updated');
				var_dump($ref->getStaticPropertyValue($name));
			}
			catch(Exception $e)
			{
				echo "EXCEPTION\n";
			}
		}
	}
}

Class TestDerived extends Test
{
//	static public    $pub = 'pub';
//	static protected $pro = 'pro';
	static private   $pri = 'pri';

	static function testing()
	{
		$ref = new ReflectionClass('Test');

		foreach(array('pub', 'pro', 'pri') as $name)
		{
			try
			{
				var_dump($ref->getStaticPropertyValue($name));
				var_dump($ref->getStaticPropertyValue($name));
				$ref->setStaticPropertyValue($name, 'updated');
				var_dump($ref->getStaticPropertyValue($name));
			}
			catch(Exception $e)
			{
				echo "EXCEPTION\n";
			}
		}
	}
}

$ref = new ReflectionClass('Test');

foreach(array('pub', 'pro', 'pri') as $name)
{
	try
	{
		var_dump($ref->getStaticPropertyValue($name));
		var_dump($ref->getStaticPropertyValue($name));
		$ref->setStaticPropertyValue($name, 'updated');
		var_dump($ref->getStaticPropertyValue($name));
	}
	catch(Exception $e)
	{
		echo "EXCEPTION\n";
	}
}

Test::testing();
TestDerived::testing();

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
unicode(3) "pub"
unicode(3) "pub"
unicode(7) "updated"
EXCEPTION
EXCEPTION
unicode(7) "updated"
unicode(7) "updated"
unicode(7) "updated"
EXCEPTION
EXCEPTION
unicode(7) "updated"
unicode(7) "updated"
unicode(7) "updated"
EXCEPTION
EXCEPTION
===DONE===
