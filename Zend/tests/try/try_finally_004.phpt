--TEST--
Try finally (without catch/finally block)
--FILE--
<?php
function foo () {
   try {
        echo "3";
   }
}

foo();
?>
--EXPECTF--
Fatal error: Cannot use try without catch or finally in %stry_finally_004.php on line %d
