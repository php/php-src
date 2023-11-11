--TEST--
JIT QM_ASSIGN: 001 incorrect optimizarion
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
echo (1%1.5)-(1.5%1%1%-1)-1;
?>
--EXPECTF--
Deprecated: Implicit conversion from float 1.5 to int loses precision in %sqm_assign_001.php on line 2

Deprecated: Implicit conversion from float 1.5 to int loses precision in %sqm_assign_001.php on line 2
-1

