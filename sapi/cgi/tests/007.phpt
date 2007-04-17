--TEST--
invalid arguments and error messages
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php
include "include.inc";

$php = get_cgi_path();
reset_env_vars();

var_dump(`"$php" -n -f some.php -f some.php`);
var_dump(`"$php" -s -w -l`);

echo "Done\n";
?>
--EXPECTF--	
string(25) "No input file specified.
"
string(31) "No syntax errors detected in -
"
Done
