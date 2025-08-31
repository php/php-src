--TEST--
Bug #77376 ("undefined function" message no longer includes namespace)
--FILE--
<?php
namespace Hello;
World();
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function Hello\World() %sbug77376.php:%d
Stack trace:
#0 {main}
  thrown in %sbug77376.php on line %d 
