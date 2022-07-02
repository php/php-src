--TEST--
php://fd wrapper: mode is ignored
--FILE--
<?php
$f = fopen("php://fd/1", "rkkk");
fwrite($f, "hi!");

echo "\nDone.\n";
?>
--EXPECT--
hi!
Done.
