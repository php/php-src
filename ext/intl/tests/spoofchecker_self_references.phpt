--TEST--
SpoofChecker with self references
--EXTENSIONS--
intl
--FILE--
<?php

$checker = new Spoofchecker();
$checker->isSuspicious("", $checker);

$checker = new Spoofchecker();
$checker->areConfusable("", "", $checker);

echo "Done\n";

?>
--EXPECT--
Done
