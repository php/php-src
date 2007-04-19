--TEST--
invalid arguments and error messages
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

var_dump(`"$php" -n -F some.php -F some.php`);
var_dump(`"$php" -n -F some.php -R some.php`);
var_dump(`"$php" -n -R some.php -F some.php`);
var_dump(`"$php" -n -R some.php -R some.php`);
var_dump(`"$php" -n -f some.php -f some.php`);
var_dump(`"$php" -n -B '' -B ''`);
var_dump(`"$php" -n -E '' -E ''`);
var_dump(`"$php" -n -r '' -r ''`);

echo "Done\n";
?>
--EXPECTF--	
string(32) "You can use -R or -F only once.
"
string(32) "You can use -R or -F only once.
"
string(32) "You can use -R or -F only once.
"
string(32) "You can use -R or -F only once.
"
string(26) "You can use -f only once.
"
string(26) "You can use -B only once.
"
string(26) "You can use -E only once.
"
string(26) "You can use -r only once.
"
Done
