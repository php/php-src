<?php #vim:ft=php
$dbname = tempnam("/tmp", "phpsql");
function cleanup() {
	sqlite_close($GLOBALS['db']);
	unlink($GLOBALS['dbname']);
}
register_shutdown_function("cleanup");
$db = sqlite_open($dbname);
?>
