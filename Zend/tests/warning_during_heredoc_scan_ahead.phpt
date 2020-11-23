--TEST--
No warnings should be thrown during heredoc scan-ahead
--FILE--
<?php

<<<TEST
\400
${"\400"}
TEST;

?>
--EXPECTF--
Warning: Octal escape sequence overflow \400 is greater than \377 in %s on line %d

Warning: Octal escape sequence overflow \400 is greater than \377 in %s on line %d

Warning: Undefined variable $ in %s on line %d
