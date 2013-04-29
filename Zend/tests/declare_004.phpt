--TEST--
Testing declare statement with several type values
--SKIPIF--
<?php
if (!ini_get("zend.multibyte")) {
  die("skip Requires zend.multibyte=1");
}
?>
--FILE--
<?php

declare(encoding = 1);
declare(encoding = 1123131232131312321);
declare(encoding = NULL);
declare(encoding = M_PI);

print 'DONE';

?>
--EXPECTF--
Warning: Unsupported encoding [%d] in %sdeclare_004.php on line 3

Warning: Unsupported encoding [%f] in %sdeclare_004.php on line 4

Warning: Unsupported encoding [] in %sdeclare_004.php on line 5

Fatal error: Cannot use constants as encoding in %sdeclare_004.php on line 6
