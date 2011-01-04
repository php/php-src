--TEST--
Testing declare statement with several type values
--INI--
zend.multibyte=1
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
Warning: Unsupported encoding [1] in %sdeclare_001.php on line %d

Warning: Unsupported encoding [1.1231312321313E+18] in %sdeclare_001.php on line %d

Warning: Unsupported encoding [] in %sdeclare_001.php on line %d

Fatal error: Cannot use constants as encoding in %sdeclare_001.php on line %d