--TEST--
Bug #61225 (Lexing 0b0*+<NUM> incorrectly)
--FILE--
<?php
echo 0b00+1;
echo "\n";
?>
--EXPECT--
1
