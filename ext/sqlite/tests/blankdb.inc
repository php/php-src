<?php #vim:ft=php
$dbname = tempnam(dirname(__FILE__), "phpsql");
function cleanup() {
	if ($GLOBALS['db']) {
		sqlite_close($GLOBALS['db']);
	}
	unlink($GLOBALS['dbname']);
}
register_shutdown_function("cleanup");
$db = sqlite_open($dbname);
?>
