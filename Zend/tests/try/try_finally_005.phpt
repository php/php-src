--TEST--
Try finally (with long goto)
--FILE--
<?php
function foo () {
   try {
   } finally {
      goto label;
   }
label:
   return 1;
}

foo();
?>
--EXPECTF--
Fatal error: jump out of a finally block is disallowed in %stry_finally_005.php on line %d
