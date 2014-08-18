--TEST--
Testing declare statement with several type values
--INI--
precision=15
zend.multibyte=1
--FILE--
<?php

declare(encoding = 1);
declare(encoding = 11111111111111);
declare(encoding = M_PI);

print 'DONE';

?>
--EXPECTF--
Warning: Unsupported encoding [1] in %sdeclare_003.php on line %d

Warning: Unsupported encoding [11111111111111] in %sdeclare_003.php on line %d

Fatal error: Encoding must be a literal in %s on line %d
