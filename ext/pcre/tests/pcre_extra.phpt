--TEST--
X (PCRE_EXTRA) modifier is ignored (no error, no change)
--FILE--
<?php

var_dump(preg_match('/\y/', '\y'));
var_dump(preg_match('/\y/X', '\y'));

?>
--EXPECTF--
Warning: preg_match(): Compilation failed: unrecognized character follows \ at offset 1 in %spcre_extra.php on line 3
bool(false)

Warning: preg_match(): Compilation failed: unrecognized character follows \ at offset 1 in %spcre_extra.php on line 4
bool(false)
