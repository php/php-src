--TEST--
php://fd wrapper: invalid file descriptor
--FILE--
<?php
fopen("php://fd/1023", "w");

echo "\nDone.\n";
--EXPECTF--
Warning: fopen(php://fd/1023): failed to open stream: Error duping file descriptor 1023; possibly it doesn't exist: [9]: %s in %s on line %d

Done.
