--TEST--
Weakref no inheritance
--FILE--
<?php
class Test extends WeakRef {}
?>
--EXPECTF--
Fatal error: Class Test may not inherit from final class (WeakRef) in %s on line %d

