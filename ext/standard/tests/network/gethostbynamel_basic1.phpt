--TEST--
Test gethostbynamel() function : basic functionality
--FILE--
<?php
echo "*** Testing gethostbynamel() : basic functionality ***\n";
var_dump(gethostbynamel("localhost"));
?>
--EXPECTF--
*** Testing gethostbynamel() : basic functionality ***
array(%d) {
  %a
}
