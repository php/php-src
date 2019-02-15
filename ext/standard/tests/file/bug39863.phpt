--TEST--
Bug #39863 (file_exists() silently truncates after a null byte)
--CREDITS--
Andrew van der Stock, vanderaj @ owasp.org
--FILE--
<?php

$filename = __FILE__ . chr(0). ".ridiculous";

if (file_exists($filename)) {
    echo "FAIL\n";
}
else {
    echo "PASS\n";
}
?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Warning: file_exists() expects parameter 1 to be a valid path, string given in %s on line %d
PASS
===DONE===
