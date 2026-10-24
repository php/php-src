--TEST--
GH-24640 (tcadb handler allows injecting Tokyo Cabinet parameters via "#" in the path)
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('tcadb');
?>
--FILE--
<?php
$db_file = __DIR__ . '/gh24640.tch';

var_dump(dba_open($db_file . '#bnum=1000', 'n-', 'tcadb'));
var_dump(file_exists($db_file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh24640.tch');
?>
--EXPECTF--
Warning: dba_open(): Driver initialization failed for handler: tcadb: Path must not contain "#" in %s on line %d
bool(false)
bool(false)
