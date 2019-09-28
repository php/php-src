--TEST--
No warnings should be thrown during heredoc scan-ahead
--FILE--
<?php

<<<TEST
${x}
\400
${/*}
TEST;

?>
--EXPECTF--
Warning: Unexpected character in input:  '' (ASCII=1) state=0 in %s on line %d

Warning: Octal escape sequence overflow \400 is greater than \377 in %s on line %d

Warning: Unterminated comment starting line %d in %s on line %d

Parse error: syntax error, unexpected end of file in %s on line %d
