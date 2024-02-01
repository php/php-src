--TEST--
JIT ASSIGN_DIM: 009
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
$y[] = $r = &$G;
?>
DONE
--EXPECT--
DONE
