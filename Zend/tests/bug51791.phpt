--TEST--
Bug #51791 (constant() failed to check undefined constant and php interpreter stopped)
--FILE--
<?php

class A  {
    const B = 1;
}
var_dump(constant('A::B1'));

?>
--EXPECTF--
Warning: constant(): Couldn't find constant A::B1 in %s on line %d
NULL
