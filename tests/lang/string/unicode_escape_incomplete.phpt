--TEST--
Invalid Unicode escape sequence: Incomplete
--FILE--
<?php

var_dunp("\u{blah");
--EXPECTF--
Parse error: Invalid UTF-8 codepoint escape sequence in %s on line %d
