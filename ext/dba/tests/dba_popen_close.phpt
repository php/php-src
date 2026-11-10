--TEST--
dba_close() releases a persistent connection once no other connection uses it
--EXTENSIONS--
dba
--SKIPIF--
<?php
$handler = "flatfile";
require_once(__DIR__ .'/skipif.inc');
if (PHP_OS_FAMILY === 'Solaris') die("skip Solaris flock() emulation cannot detect a lock held by the same process");
?>
--CONFLICTS--
dba
--FILE--
<?php
$handler = "flatfile";
require_once(__DIR__ .'/test.inc');

function lock_free(string $file): bool {
    $db = @dba_open($file, 'wlt', 'flatfile');
    if ($db === false) {
        return false;
    }
    dba_close($db);
    return true;
}

$db1 = dba_popen($db_filename, 'cl', 'flatfile');
dba_insert('key', 'value', $db1);
$db2 = dba_popen($db_filename, 'cl', 'flatfile');

dba_close($db2);
echo "first still open: ", dba_fetch('key', $db1), "\n";
echo "lock free after closing one of two: "; var_dump(lock_free($db_filename));

dba_close($db1);
echo "lock free after closing both: "; var_dump(lock_free($db_filename));

$db3 = dba_popen($db_filename, 'cl', 'flatfile');
unset($db3);
$db4 = dba_popen($db_filename, 'cl', 'flatfile');
echo "reused after implicit release: ", dba_fetch('key', $db4), "\n";
dba_close($db4);
echo "lock free after closing the reused connection: "; var_dump(lock_free($db_filename));

$db5 = dba_popen($db_filename, 'cl', 'flatfile');
$db6 = dba_popen($db_filename, 'cl', 'flatfile');
dba_close($db5);
var_dump(@dba_open($db_filename, 'wlt', 'flatfile'));
echo "conflict seen after closing the earlier connection: ", error_get_last()['message'], "\n";
dba_close($db6);

$reader1 = dba_popen($db_filename, 'rl', 'flatfile');
$reader2 = dba_popen($db_filename, 'rd', 'flatfile');
dba_close($reader1);
echo "connection sharing the stream still reads: ", dba_fetch('key', $reader2), "\n";
dba_close($reader2);
?>
--CLEAN--
<?php
require(__DIR__ .'/clean.inc');
?>
--EXPECT--
first still open: value
lock free after closing one of two: bool(false)
lock free after closing both: bool(true)
reused after implicit release: value
lock free after closing the reused connection: bool(true)
bool(false)
conflict seen after closing the earlier connection: dba_open(): Driver initialization failed for handler: flatfile: Unable to establish lock (database file already open)
connection sharing the stream still reads: value
