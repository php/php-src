--TEST--
no outer class
--FILE--
<?php

new Outer:>Inner();
?>
--EXPECTF--
Fatal error: Outer class 'Outer' not found for inner class Outer:>Inner in %s on line %d
