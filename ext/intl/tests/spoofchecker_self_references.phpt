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
[Sat Jun  1 19:32:47 2024]  Script:  '/run/media/niels/MoreData/php-8.2/ext/intl/tests/spoofchecker_self_references.php'
/run/media/niels/MoreData/php-8.2/Zend/zend_objects_API.h(92) :  Freeing 0x0000796cc9205780 (64 bytes), script=/run/media/niels/MoreData/php-8.2/ext/intl/tests/spoofchecker_self_references.php
Last leak repeated 1 time
=== Total 2 memory leaks detected ===
