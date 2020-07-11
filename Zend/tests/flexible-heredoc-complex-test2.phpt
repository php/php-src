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
--EXPECT--
string(5) "a
b
c"
