--TEST--
Testing declare statement with several type values
--SKIPIF--
<?php
if (!ini_get("zend.multibyte")) {
  die("skip Requires zend.multibyte=1");
}
if (!extension_loaded("mbstring")) {
  die("skip Requires ext/mbstring");
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
Warning: Unsupported encoding [%d] in %sdeclare_002.php on line 3

Warning: Unsupported encoding [%f] in %sdeclare_002.php on line 4

Warning: Unsupported encoding [] in %sdeclare_002.php on line 5

Fatal error: Cannot use constants as encoding in %sdeclare_002.php on line 7
