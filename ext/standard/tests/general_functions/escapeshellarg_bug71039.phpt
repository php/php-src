--TEST--
Test escapeshellarg() string with \0 bytes
--FILE--
<?php
escapeshellarg("hello\0world");

?>
===DONE===
--EXPECTF--
Fatal error: escapeshellarg(): Input string contains NULL bytes in %s on line %d
