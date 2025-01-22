--TEST--
GH-14553 (Bug in phpdbg8.3 (also 8.1 and 8.2) echo output - trimmed at NULL byte (?))
--PHPDBG--
r
q
--FILE--
<?php
echo "hello\0world";
?>
--EXPECTF--
[Successful compilation of %s]
prompt> hello%0world
[Script ended normally]
prompt>
