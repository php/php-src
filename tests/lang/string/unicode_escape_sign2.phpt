--TEST--
Invalid Unicode escape sequence: Negative sign
--FILE--
<?php

var_dump("\u{-1F602}");
?>
--EXPECTF--
Parse error: Invalid UTF-8 codepoint escape sequence in %s on line %d
