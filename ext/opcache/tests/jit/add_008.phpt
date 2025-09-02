--TEST--
JIT ADD: 008 Addition with reference IS_VAR
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
opcache.jit=tracing
--EXTENSIONS--
opcache
--FILE--
<?php
($a =& $b) + ($a = 1);
?>
DONE
--EXPECT--
DONE
