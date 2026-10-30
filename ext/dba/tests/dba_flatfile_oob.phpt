--TEST--
DBA FlatFile handler bounds with a malformed (negative) length field
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('flatfile');
?>
--FILE--
<?php
$db_file = __DIR__ . '/dba_flatfile_oob.db';
// A negative length narrows to a huge size_t and previously overran the read buffer.
file_put_contents($db_file, "-1\n" . str_repeat('A', 200000));

$db = dba_open($db_file, 'r', 'flatfile');
var_dump(dba_firstkey($db));
var_dump(dba_exists("AAAA", $db));
var_dump(dba_fetch("AAAA", $db));
dba_close($db);
echo "done\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/dba_flatfile_oob.db');
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
done
