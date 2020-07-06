--TEST--
WeakReference no inheritance
--FILE--
<?php
class Test extends WeakReference {}
?>
--EXPECTF--
Fatal error: Class Test cannot extend final class WeakReference in %s on line %d
