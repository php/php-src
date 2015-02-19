--TEST--
Comparable: compareTo() method not implemented
--FILE--
<?php

class C implements Comparable {}

?>
--EXPECTF--
Fatal error: Class C contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (Comparable::compareTo) in %s on line %d
