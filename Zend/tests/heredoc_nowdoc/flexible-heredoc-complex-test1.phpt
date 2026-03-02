--TEST--
Flexible heredoc syntax complex test 1: interpolated nested heredocs
with different delimiter names
--FILE--
<?php

$a = 'b';
${"b\nb\n d"} = 'b';

var_dump(<<<DOC1
    a
    ${<<<DOC2
        b
        ${<<<DOC3
            a
            DOC3}
         d
        DOC2
    }
    c
    DOC1);

?>
--EXPECTF--
Deprecated: Using ${expr} (variable variables) in strings is deprecated, use {${expr}} instead in %s on line %d

Deprecated: Using ${expr} (variable variables) in strings is deprecated, use {${expr}} instead in %s on line %d
string(5) "a
b
c"
