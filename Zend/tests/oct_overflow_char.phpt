--TEST--
Octal overflow in string interpolation
--FILE--
<?php

// "abc", ordinarily 'b' would be \142, but we'll deliberately overflow the value by \400
echo "\141\542\143\n";
?>
--EXPECTF--
Warning: Octal escape sequence overflow \542 is greater than \377 in %s%eoct_overflow_char.php on line 4
abc
