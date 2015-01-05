--TEST--
Bug #61095 (Lexing 0x00*+<NUM> incorectly)
--FILE--
<?php
echo 0x00+2;
echo "\n";
?>
--EXPECT--
2
