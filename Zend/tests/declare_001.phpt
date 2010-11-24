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
declare(encoding = M_PI);
declare(encoding = 'utf-8');

print 'DONE';

?>
--EXPECT--
DONE
