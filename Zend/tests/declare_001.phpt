--TEST--
Testing declare statement with several type values
--SKIPIF--
<?php
if (ini_get("zend.multibyte")) {
  die("skip Requires zend.multibyte=0");
}
?>
--FILE--
<?php

declare(encoding = 1);
declare(encoding = 1123131232131312321);
declare(encoding = NULL);
declare(encoding = 'utf-8');
declare(encoding = M_PI);

print 'DONE';

?>
--EXPECTF--
Fatal error: Cannot use constants as encoding in %sdeclare_001.php on line 7
