--TEST--
A sub-class of a locked class is not locked unless explciitly marked
--FILE--
<?php

locked class Base {
	public $definedProp;
}
class UnlockedDerived extends Base {
}
locked class LockedDerived extends Base {
}

$t = new UnlockedDerived();

$t->definedProp = "OK\n";
echo $t->definedProp;
unset($t->definedProp);

$t->nonExistentProp = "Also OK\n";
echo $t->nonExistentProp;
unset($t->nonExistentProp);

  
$t = new LockedDerived();

$t->definedProp = "OK\n";
$t->nonExistentProp = "Not OK\n";


echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
OK
Also OK

Fatal error: Uncaught Error: Cannot write undefined property %s on locked class %s in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
