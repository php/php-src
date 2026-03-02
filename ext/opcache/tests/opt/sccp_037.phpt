--TEST--
SCCP 037: Memory leak
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
[!![[new ERROR]]];
?>
DONE
--EXPECT--
DONE
