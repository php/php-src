--TEST--
Unsetting and recreating protected properties.
--FILE--
<?php
class C {
	protected $p = 'test';
	function unsetProtected() {
		unset($this->p);		
	}
	function setProtected() {
		$this->p = 'changed';		
	}
}

class D extends C {
	function setP() {
		$this->p = 'changed in D';
	}
}

$d = new D;
echo "Unset and recreate a protected property from property's declaring class scope:\n";
$d->unsetProtected();
$d->setProtected();
var_dump($d);

echo "\nUnset and recreate a protected property from subclass:\n";
$d = new D;
$d->unsetProtected();
$d->setP();
var_dump($d);

echo "\nUnset a protected property, and attempt to recreate it outside of scope (expected failure):\n";
$d->unsetProtected();
$d->p = 'this will fail';
var_dump($d);
?>
--EXPECTF--
Unset and recreate a protected property from property's declaring class scope:
object(D)#%d (1) {
  [%u|b%"p":protected]=>
  %unicode|string%(7) "changed"
}

Unset and recreate a protected property from subclass:
object(D)#%d (1) {
  [%u|b%"p":protected]=>
  %unicode|string%(12) "changed in D"
}

Unset a protected property, and attempt to recreate it outside of scope (expected failure):

Fatal error: Uncaught Error: Cannot access protected property %s::$p in %s:32
Stack trace:
#0 {main}
  thrown in %s on line 32
