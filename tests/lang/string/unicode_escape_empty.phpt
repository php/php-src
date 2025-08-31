--TEST--
Invalid Unicode escape sequence: Empty
--FILE--
<?php

var_dump("\u{}");
?>
--EXPECTF--
Parse error: Invalid UTF-8 codepoint escape sequence in %s on line %d
