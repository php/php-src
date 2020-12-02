--TEST--
Bug #39863 (file_exists() silently truncates after a null byte)
--CREDITS--
Andrew van der Stock, vanderaj @ owasp.org
--FILE--
<?php

$filename = __FILE__ . chr(0). ".ridiculous";
var_dump(file_exists($filename));
?>
--EXPECT--
bool(false)
