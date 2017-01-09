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

Warning: Unexpected character in input:  '%s' (ASCII=127) state=0 in %s(%d) : eval()'d code on line %d

Parse error: syntax error, unexpected 'b' (T_STRING) in %s(%d) : eval()'d code on line %d
