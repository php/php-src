--TEST--
Bug #69899: Segfault on stmt close after free_result with mysqlnd.
--DESCRIPTION--
The segfault happens only if the database connection was already closed and
free_result is called on a prepared statement followed by closing that
statement. This is due to mysqlnd_stmt::free_result (mysqlnd_ps.c) which
unconditionally sets the connection of the statement to ready, despite the fact
that it might already be closed.
--SKIPIF--
<?php
require_once __DIR__ . '/skipif.inc';
require_once __DIR__ . '/skipifconnectfailure.inc';
require_once __DIR__ . '/connect.inc';
if (!$IS_MYSQLND) {
	die('mysqlnd only');
}
?>
--FILE--
<?php

require_once __DIR__ . '/connect.inc';

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

$mysqli = new mysqli($host, $user, $passwd, $db, $port, $socket);
$stmt   = $mysqli->prepare('SELECT 1');

var_dump(
	$mysqli->close(),
	$stmt->free_result(),
	$stmt->close()
);

?>
--EXPECT--
bool(true)
NULL
bool(true)
