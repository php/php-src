<?php #vim:ft=php
$dbname = tempnam(dirname(__FILE__), "phpsql");
function cleanup() {
	if (is_resource($GLOBALS['db'])) {
		@sqlite_close($GLOBALS['db']);
		usleep(500000);
	}
	unlink($GLOBALS['dbname']);
}
register_shutdown_function("cleanup");
$db = sqlite_open($dbname);
?>
