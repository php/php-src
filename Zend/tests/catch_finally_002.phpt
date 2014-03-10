--TEST--
Try catch finally (basic test with return)
--FILE--
<?php
function foo () {
   try {
     echo "try\n";
     return 1;
   } catch (Exception $e) {
   } finally {
     echo "finally\n";
   }
   return 2;
}

var_dump(foo());
?>
--EXPECTF--
try
finally
int(1)
