--TEST--
Finally with near goto
--FILE--
<?php
function foo () {
   try {
   } finally {
      goto label;
      echo "dummy";
label:
      echo "label";
   }
}

foo();
?>
--EXPECTF--
label
