<?php #vim:ft=php
$dbname = tempnam(dirname(__FILE__), "phpsql");
function cleanup() {
	global $db, $dbname;
	$db = NULL;
	usleep(500000);
	@unlink($dbname);
}
register_shutdown_function("cleanup");
$db = new SQLiteDatabase($dbname);
?>
