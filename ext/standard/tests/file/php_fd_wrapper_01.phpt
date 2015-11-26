--TEST--
php://fd wrapper: basic test
--FILE--
<?php
$f = fopen("php://fd/1", "wb");
fwrite($f, "hi!");

echo "\nDone.\n";
--EXPECT--
hi!
Done.
