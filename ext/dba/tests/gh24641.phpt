--TEST--
GH-24641 (ndbm handler does not check dbm_open() result)
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('ndbm');
?>
--FILE--
<?php
$file = __DIR__ . '/gh24641.db';
file_put_contents($file, 'not a valid ndbm database');
var_dump(dba_open($file, 'r', 'ndbm'));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh24641.db');
?>
--EXPECTF--
Warning: dba_open(): Driver initialization failed for handler: ndbm in %s on line %d
bool(false)
