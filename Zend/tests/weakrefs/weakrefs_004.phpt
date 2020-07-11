--TEST--
WeakReference no inheritance
--FILE--
<?php
class Test extends WeakReference {}
?>
--EXPECTF--
Fatal error: Class Test may not inherit from final class (WeakReference) in %s on line %d
