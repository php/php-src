--TEST--
WeakReference no __construct
--FILE--
<?php
new WeakReference();
?>
--EXPECTF--
Fatal error: Uncaught Error: Direct instantiation of WeakReference is not allowed, use WeakReference::create instead in %s:2
Stack trace:
#0 %s(2): WeakReference->__construct()
#1 {main}
  thrown in %s on line 2
