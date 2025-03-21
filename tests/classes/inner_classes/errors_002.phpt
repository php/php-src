--TEST--
inner class not found
--FILE--
<?php

class Outer {
}

new Outer:>Inner();
?>
--EXPECTF--
Fatal error: Inner class 'Outer:>Inner' not found in outer class Outer in %s on line %d
