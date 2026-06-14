--TEST--
Const NAN with bool cast should emit 1 warning
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php
echo (bool)NAN;
?>
--EXPECTF--
Warning: unexpected NAN value was coerced to bool in %s on line 2
1
