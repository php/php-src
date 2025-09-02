--TEST--
Block Pass 004: Inorrect QM_ASSIGN elimination
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
(1?4:y)?->y;
?>
DONE
--EXPECTF--
Warning: Attempt to read property "y" on int in %sblock_pass_004.php on line 2
DONE
