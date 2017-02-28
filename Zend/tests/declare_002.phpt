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
declare(encoding = 1123131232131312321);
declare(encoding = 'utf-8');
declare(encoding = M_PI);

print 'DONE';

?>
--EXPECTF--
Warning: Unsupported encoding [%d] in %sdeclare_002.php on line 3

Warning: Unsupported encoding [%f] in %sdeclare_002.php on line 4

Parse error: syntax error, unexpected 'M_PI' (T_STRING), expecting integer number (T_LNUMBER) or floating-point number (T_DNUMBER) or quoted-string (T_CONSTANT_ENCAPSED_STRING) in %s/declare_002.php on line 6
