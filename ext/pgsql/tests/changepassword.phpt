--TEST--
Changing user password with pg_change_password
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include('inc/config.inc');

$conn = pg_connect($conn_str);

try {
	pg_change_password($conn, "", "pass");
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	pg_change_password($conn, "user", "");
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

var_dump(pg_change_password($conn, "inexistent_user", "postitpwd"));
?>
--EXPECT--
pg_change_password(): Argument #2 ($user) must not be empty
pg_change_password(): Argument #3 ($password) must not be empty
bool(false)
