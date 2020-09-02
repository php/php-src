--TEST--
Bug #71897 (ASCII 0x7F Delete control character permitted in identifiers)
--FILE--
<?php

eval("
    \$a\x7Fb = 3;
    var_dump(\$a\x7Fb);
");

?>
--EXPECTF--
Parse error: syntax error, unexpected character 0x7F in %s on line %d
