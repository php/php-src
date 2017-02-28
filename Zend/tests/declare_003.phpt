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

Parse error: syntax error, unexpected 'M_PI' (T_STRING), expecting integer number (T_LNUMBER) or floating-point number (T_DNUMBER) or quoted-string (T_CONSTANT_ENCAPSED_STRING) in %s/declare_003.php on line 5
