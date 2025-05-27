--TEST--
DBA open same read only file multiple times
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('cdb_make');
?>
--CONFLICTS--
test.cdb
--FILE--
<?php
echo "database handler: cdb\n";
$handler = 'cdb';
$db_filename = __DIR__.'/test.cdb';
if (($db_file=dba_open($db_filename, "r", $handler))!==FALSE) {
    echo dba_fetch(1, $db_file);
    if (($db_file2=dba_open($db_filename, "r", $handler))!==FALSE) {
        echo dba_fetch(1, $db_file2);
        echo dba_fetch(2, $db_file2);
        dba_close($db_file2);
    } else {
        echo "Error opening database 2nd time\n";
    }
    echo dba_fetch(2, $db_file);
    dba_close($db_file);
} else {
    echo "Error opening database\n";
}
?>
--EXPECT--
database handler: cdb
1122
