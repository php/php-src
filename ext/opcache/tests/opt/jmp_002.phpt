--TEST--
JMP 002: JMP around unreachable FREE
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
7??match(X){};
?>
DONE
--EXPECT--
DONE
