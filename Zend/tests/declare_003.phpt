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
Warning: Encoding "1" is unsupported in %s on line %d

Warning: Encoding "11111111111111" is unsupported in %s on line %d

Fatal error: Encoding must be a literal in %s on line %d
