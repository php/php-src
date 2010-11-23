--TEST--
Testing declare statement with several type values
--SKIPIF--
<?php
if (in_array("detect_unicode", array_keys(ini_get_all()))) {
  die("skip");
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
