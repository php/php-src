--TEST--
GH-22046: The unserialize function can lead to segfault when internal classes are serialized back with the unsupported C format
--FILE--
<?php

$payload = 'C:14:"Uri\WhatWg\Url":0:{}';
unserialize($payload);

$payload = 'C:15:"Uri\Rfc3986\Uri":0:{}';
unserialize($payload);
?>
--EXPECTF--
Warning: Class Uri\WhatWg\Url has no unserializer in %s on line %d

Warning: unserialize(): Error at offset 25 of 26 bytes in %s on line %d

Warning: Class Uri\Rfc3986\Uri has no unserializer in %s on line %d

Warning: unserialize(): Error at offset 26 of 27 bytes in %s on line %d
