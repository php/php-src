--TEST--
Testing declare statement with several type values
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
--FILE--
<?php

declare(encoding = 1);
declare(encoding = 1123131232131312321);
declare(encoding = 'utf-8');
declare(encoding = M_PI);

print 'DONE';

?>
--EXPECTF--
Warning: Unsupported encoding [%d] in %sdeclare_002.php on line 3

Warning: Unsupported encoding [%f] in %sdeclare_002.php on line 4

Fatal error: Encoding must be a literal in %sdeclare_002.php on line 6
