--TEST--
Bug #73210: Segfault with stmt read only cursor and get_result due to double closing
--DESCRIPTION--
Getting the result of an executed prepared statement that uses a read only
cursor results in a segfault because the second close call tries to free the
internal result on null. It does not matter which is the first or second close
call since stmt gives result a pointer to the result, the one that calls it
first is the one that frees it and the other one accesses null.
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

$stmt = $mysqli->prepare('SELECT 1 UNION SELECT 2 UNION SELECT 3');
$stmt->attr_set(MYSQLI_STMT_ATTR_CURSOR_TYPE, MYSQLI_CURSOR_TYPE_READ_ONLY);
$stmt->execute();
$result = $stmt->get_result();

// order of invocation does not matter {{{
$result->close();
$stmt->close();
// }}}

$mysqli->close();

echo 'OK';

?>
--EXPECT--
OK
