--TEST--
Errors: forward reference in default of earlier parameter
--FILE--
<?php
class C<U = T, T = int> {}
?>
--EXPECTF--
Fatal error: Type parameter T referenced before declaration in %s on line %d
