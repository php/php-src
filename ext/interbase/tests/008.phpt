--TEST--
InterBase: event handling
--SKIPIF--
<?php 
if (PHP_OS == "WINNT") echo "skip";
include("skipif.inc"); 
?>
--FILE--
<?php /* $Id$ */

require("interbase.inc");

$count = 0;

function event_callback($event)
{
	global $count;
	if ($event == 'TEST1') echo "FAIL TEST1\n";
	return (++$count < 5); /* cancel event */
}

$link = ibase_connect($test_base);

ibase_query("CREATE PROCEDURE pevent AS BEGIN POST_EVENT 'TEST1'; POST_EVENT 'TEST2'; END");
ibase_commit();

$e = ibase_set_event_handler('event_callback','TEST1');
ibase_free_event_handler($e);

ibase_set_event_handler('event_callback','TEST2');

usleep(5E+5);

for ($i = 0; $i < 8; ++$i) {
	ibase_query("EXECUTE PROCEDURE pevent");
	ibase_commit();

	usleep(3E+5);
}

usleep(5E+5);

if (!$count || $count > 5) echo "FAIL ($count)\n";
echo "end of test\n";

?>
--EXPECT--
end of test
