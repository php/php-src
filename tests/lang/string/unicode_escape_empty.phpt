--TEST--
Invalid Unicode escape sequence: Empty
--FILE--
<?php

var_dump("\u{}");
--EXPECTF--
Fatal error: Invalid UTF-8 codepoint escape sequence in %s on line %d
