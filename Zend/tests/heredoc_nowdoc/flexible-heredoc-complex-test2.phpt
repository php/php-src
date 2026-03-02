--TEST--
Flexible heredoc syntax complex test 2: interpolated nested heredocs
with the same delimiter name
--FILE--
<?php

$a = 'b';
${"b\nb\n d"} = 'b';

var_dump(<<<DOC1
    a
    ${<<<DOC1
        b
        ${<<<DOC1
            a
            DOC1}
         d
        DOC1
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
