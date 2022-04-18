--TEST--
FE_RESET: 001 undef $$ operand
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
for ($i = 0; $i < 5; $i++) {
    for ($j = 0; $j < $i; $j++) {}
    foreach ($$i as $x) {}
}
?>
OK
--EXPECTF--
Warning: Undefined variable $0 in %sfe_reset_001.php on line 4

Warning: foreach() argument must be of type array|object, null given in %sfe_reset_001.php on line 4

Warning: Undefined variable $1 in %sfe_reset_001.php on line 4

Warning: foreach() argument must be of type array|object, null given in %sfe_reset_001.php on line 4

Warning: Undefined variable $2 in %sfe_reset_001.php on line 4

Warning: foreach() argument must be of type array|object, null given in %sfe_reset_001.php on line 4

Warning: Undefined variable $3 in %sfe_reset_001.php on line 4

Warning: foreach() argument must be of type array|object, null given in %sfe_reset_001.php on line 4

Warning: Undefined variable $4 in %sfe_reset_001.php on line 4

Warning: foreach() argument must be of type array|object, null given in %sfe_reset_001.php on line 4
OK

