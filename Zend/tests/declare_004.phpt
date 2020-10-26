--TEST--
Testing declare statement with several type values
--INI--
zend.multibyte=1
--FILE--
<?php

declare(encoding = 1);
declare(encoding = 1123131232131312321);
declare(encoding = M_PI);

print 'DONE';

?>
--EXPECTF--
Warning: Unsupported encoding 1 in %s on line %d

Warning: Unsupported encoding 1123131232131312321 in %s on line %d

Fatal error: Encoding must be a literal in %sdeclare_004.php on line 5
