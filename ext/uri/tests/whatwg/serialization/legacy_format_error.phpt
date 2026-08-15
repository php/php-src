--TEST--
GH-22046: Uri\WhatWg\Url cannot be unserialized with the unsupported C format
--FILE--
<?php

unserialize('C:14:"Uri\WhatWg\Url":0:{}');

?>
--EXPECTF--
Warning: Class Uri\WhatWg\Url has no unserializer in %s on line %d

Warning: unserialize(): Error at offset 25 of 26 bytes in %s on line %d
