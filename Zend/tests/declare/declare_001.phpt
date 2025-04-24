--TEST--
Testing declare statement with several type values
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
--FILE--
<?php

declare(encoding = 1);
declare(encoding = 112313123213131232100);
declare(encoding = 'utf-8');
declare(encoding = M_PI);

print 'DONE';

?>
--EXPECTF--
Warning: Unsupported encoding [1] in %sdeclare_001.php on line %d

Warning: Unsupported encoding [1.1231312321313E+20] in %sdeclare_001.php on line %d

Fatal error: Encoding must be a literal in %s on line %d
