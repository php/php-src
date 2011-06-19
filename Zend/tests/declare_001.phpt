--TEST--
Testing declare statement with several type values
--SKIPIF--
<?php
if (!extension_loaded("mbstring")) {
  die("skip Requires ext/mbstring");
}
?>
--INI--
zend.multibyte=1
--FILE--
<?php

declare(encoding = 1);
declare(encoding = 112313123213131232100);
declare(encoding = NULL);
declare(encoding = 'utf-8');
declare(encoding = M_PI);

print 'DONE';

?>
--EXPECTF--
Warning: Unsupported encoding [1] in %sdeclare_001.php on line %d

Warning: Unsupported encoding [1.1231312321313E+20] in %sdeclare_001.php on line %d

Warning: Unsupported encoding [] in %sdeclare_001.php on line %d

Fatal error: Cannot use constants as encoding in %sdeclare_001.php on line %d
