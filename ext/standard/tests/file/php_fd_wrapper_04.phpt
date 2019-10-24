--TEST--
php://fd wrapper: invalid file descriptor
--FILE--
<?php
fopen("php://fd/120", "w");

echo "\nDone.\n";
--EXPECTF--
Warning: fopen(php://fd/120): failed to open stream: Error duping file descriptor 120; possibly it doesn't exist: [9]: %s in %s on line %d

Done.
