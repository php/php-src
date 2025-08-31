--TEST--
SCCP 034: memory leak
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
is_array(["$y $y"]);
?>
DONE
--EXPECTF--
Warning: Undefined variable $y in %ssccp_034.php on line 2

Warning: Undefined variable $y in %ssccp_034.php on line 2
DONE
