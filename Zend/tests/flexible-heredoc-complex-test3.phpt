--TEST--
Flexible heredoc syntax complex test 3: interpolated nested heredocs
with the same delimiter name with different levels of indentation
--FILE--
<?php

${' a'} = ' b';
${' b'} = 'c';
${"b\n b"} = 'b';

var_dump(<<<DOC1
      a
     ${<<<DOC2
        b
        ${<<<DOC3
             a
            DOC3}
        DOC2
     }
    c
    DOC1);

?>
--EXPECTF--
Deprecated: Using ${expr} (variable variables) in strings is deprecated, use {${expr}} instead in %s on line %d

Deprecated: Using ${expr} (variable variables) in strings is deprecated, use {${expr}} instead in %s on line %d
string(8) "  a
 b
c"
