<?php #vim:ft=php
$dbname = tempnam(dirname(__FILE__), "phpsql");
function cleanup() {
	$retry = 10;
	
	if (is_resource($GLOBALS['db'])) {
		@sqlite_close($GLOBALS['db']);
	}
	do {
		usleep(500000);
		if (@unlink($GLOBALS['dbname']))
			break;
	} while (file_exists($GLOBALS['dbname']) && --$retry);
}
register_shutdown_function("cleanup");
$db = sqlite_open($dbname);
?>
