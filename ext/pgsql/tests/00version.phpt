--TEST--
PostgreSQL version
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
// Get postgresql version for easier debugging.
// Execute run-test.php with --keep-all to get version string in 00version.log or 00version.out
include('config.inc');

$db = pg_connect($conn_str);
var_dump(pg_version($db));
pg_close($db);

// Get environment vars for debugging
var_dump(serialize($_ENV));

echo "OK";
?>
--EXPECTF--
array(13) {
  ["client"]=>
  string(%d) "%s"
  ["protocol"]=>
  int(%d)
  ["server"]=>
  string(%d) "%s"
  ["server_encoding"]=>
  string(%d) "%s"
  ["client_encoding"]=>
  string(%d) "%s"
  ["is_superuser"]=>
  string(%d) "%s"
  ["session_authorization"]=>
  string(%d) "%s"
  ["DateStyle"]=>
  string(%d) "%s"
  ["IntervalStyle"]=>
  string(%d) %s
  ["TimeZone"]=>
  string(%d) "%s"
  ["integer_datetimes"]=>
  string(%d) "%s"
  ["standard_conforming_strings"]=>
  string(%d) "%s"
  ["application_name"]=>
  string(%d) %s
}
string(%d) "%a"
OK
