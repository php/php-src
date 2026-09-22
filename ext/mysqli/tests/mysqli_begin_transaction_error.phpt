--TEST--
mysqli_begin_transaction()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';

require_once 'connect.inc';
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
    die(sprintf("skip Cannot connect, [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

if (!have_innodb($link))
    die(sprintf("skip Needs InnoDB support, [%d] %s", $link->errno, $link->error));
?>
--FILE--
<?php
require_once 'connect.inc';

mysqli_report(MYSQLI_REPORT_ALL & ~MYSQLI_REPORT_INDEX);
$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
try {
    mysqli_query($link, sprintf('KILL %d', mysqli_thread_id($link)));
} catch (mysqli_sql_exception) {
    // ignore
}

try {
    mysqli_begin_transaction($link);
} catch (mysqli_sql_exception $e) {
    echo "Expecting an exception.\n";
}

echo "done!\n";
?>
--CLEAN--
<?php
    require_once 'clean_table.inc';
?>
--EXPECT--
Expecting an exception.
done!
