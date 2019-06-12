--TEST--
php://fd wrapper: invalid file descriptor
--FILE--
<?php
fopen("php://fd/12", "w");

echo "\nDone.\n";
--EXPECTF--
Warning: fopen(): failed to open stream (php://fd/12): Error duping file descriptor 12; possibly it doesn't exist: [9]: %s in %s on line %d

Done.
