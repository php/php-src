--TEST--
Class protected constant visibility error
--FILE--
<?php
class A {
	protected const protectedConst = 'protectedConst';
}

var_dump(A::protectedConst);

?>
--EXPECTF--
Fatal error: Cannot access protected const A::protectedConst in %s on line 6
