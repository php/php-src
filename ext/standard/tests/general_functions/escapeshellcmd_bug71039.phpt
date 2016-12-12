--TEST--
Test escapeshellcmd() string with \0 bytes
--FILE--
<?php
escapeshellcmd("hello\0world");

?>
===DONE===
--EXPECTF--
Fatal error: escapeshellcmd(): Input string contains NULL bytes in %s on line %d
