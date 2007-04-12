--TEST--
Bug #39001 (ReflectionProperty returns incorrect declaring class for protected properties)
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

class Meta {
}

class CParent extends Meta {
	public $publicVar;
	protected $protectedVar;
}

class Child extends CParent {
}

$r = new ReflectionClass('Child');

var_dump($r->getProperty('publicVar')->getDeclaringClass()->getName());
var_dump($r->getProperty('protectedVar')->getDeclaringClass()->getName());

echo "Done\n";
?>
--EXPECTF--	
string(7) "CParent"
string(7) "CParent"
Done
