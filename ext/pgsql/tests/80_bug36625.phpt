--TEST--
Bug #36625 (8.0+) (pg_trace() does not work)
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php

require_once('config.inc');

$dbh = @pg_connect($conn_str);
if (!$dbh) {
	die ('Could not connect to the server');
}

$tracefile = __DIR__ . '/trace.tmp';

@unlink($tracefile);
var_dump(file_exists($tracefile));

pg_trace($tracefile, 'w', $dbh);
$res = pg_query($dbh, 'select 1');
var_dump($res);
pg_close($dbh);

$found = 0;
function search_trace_file($line)
{
	if (strpos($line, '"select 1"') !== false || strpos($line, "'select 1'") !== false) {
		$GLOBALS['found']++;
	}
}

$trace = file($tracefile);
array_walk($trace, 'search_trace_file');
var_dump($found > 0);
var_dump(file_exists($tracefile));

@unlink($tracefile);

?>
===DONE===
--CLEAN--
<?php

$tracefile = __DIR__ . '/trace.tmp';

unlink($tracefile);

?>
--EXPECTF--
bool(false)
resource(%d) of type (pgsql result)
bool(true)
bool(true)
===DONE===
