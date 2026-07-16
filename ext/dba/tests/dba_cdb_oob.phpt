--TEST--
DBA CDB handler bounds a record length that exceeds the file
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('cdb');
?>
--FILE--
<?php
$db_file = __DIR__ . '/dba_cdb_oob.db';

// 2048-byte hash-table header, then a first record whose key-length field is
// 0xFFFFFFFF. That length drove zend_string_alloc() directly; on 32-bit it
// wrapped the struct size to a few bytes while the read copied the trailing
// data past it. eod (bytes 0-3) must exceed 2048 so the record is reached.
$buf = str_repeat("\0", 2048 + 8);
$buf[0] = "\x00"; $buf[1] = "\x00"; $buf[2] = "\x10"; $buf[3] = "\x00"; // eod = 1 MiB
$buf[2048] = "\xff"; $buf[2049] = "\xff"; $buf[2050] = "\xff"; $buf[2051] = "\xff"; // klen
$buf .= str_repeat("A", 8192);
file_put_contents($db_file, $buf);

$db = dba_open($db_file, 'r', 'cdb');
var_dump(dba_firstkey($db));
dba_close($db);
echo "done\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/dba_cdb_oob.db');
?>
--EXPECT--
bool(false)
done
