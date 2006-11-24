--TEST--
invalid arguments and error messages
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$php = $_ENV['TEST_PHP_EXECUTABLE'];

var_dump(`"$php" -F some.php -F some.php`);
var_dump(`"$php" -F some.php -R some.php`);
var_dump(`"$php" -R some.php -F some.php`);
var_dump(`"$php" -R some.php -R some.php`);
var_dump(`"$php" -f some.php -f some.php`);
var_dump(`"$php" -B '' -B ''`);
var_dump(`"$php" -E '' -E ''`);
var_dump(`"$php" -r "" -r ""`);

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
