<?php #vim:ft=php
$dbname = tempnam(dirname(__FILE__), "phpsql");
function cleanup() {
	global $db, $dbname;

	$db = NULL;
	unlink($dbname);
}
register_shutdown_function("cleanup");
$db = new sqlite_db($dbname);
?>
