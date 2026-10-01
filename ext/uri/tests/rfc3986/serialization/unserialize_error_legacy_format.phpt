--TEST--
Test Uri\Rfc3986\Uri unserialize() - error - unsupported C format
--FILE--
<?php

unserialize('C:15:"Uri\Rfc3986\Uri":0:{}');
?>
--EXPECTF--
Warning: Class Uri\Rfc3986\Uri has no unserializer in %s on line %d

Warning: unserialize(): Error at offset 26 of 27 bytes in %s on line %d
