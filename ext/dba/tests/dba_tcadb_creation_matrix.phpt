--TEST--
DBA tcadb opening matrix of combination
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('tcadb');
?>
--FILE--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';

$handler = 'tcadb';
run_creation_tests($handler);

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$handler = 'tcadb';
clean_creation_tests($handler);
?>
--EXPECTF--
=== OPENING NON-EXISTING FILE ===
Mode parameter is "rl":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "rd":

Warning: dba_open(tcadb_not_existing.tch): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "r-":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "r":

Warning: dba_open(tcadb_not_existing.tch): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "wl":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "wd":

Warning: dba_open(tcadb_not_existing.tch): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "w-":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "w":

Warning: dba_open(tcadb_not_existing.tch): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "cl":
This is a test insert
Mode parameter is "cd":
This is a test insert
Mode parameter is "c-":
This is a test insert
Mode parameter is "c":
This is a test insert
Mode parameter is "nl":
This is a test insert
Mode parameter is "nd":
This is a test insert
Mode parameter is "n-":
This is a test insert
Mode parameter is "n":
This is a test insert
=== OPENING EXISTING DB FILE ===
Mode parameter is "rl":

Warning: dba_insert(): Cannot perform a modification on a readonly database in %s on line %d
Insertion failed
Mode parameter is "rd":

Warning: dba_insert(): Cannot perform a modification on a readonly database in %s on line %d
Insertion failed
Mode parameter is "r-":

Warning: dba_insert(): Cannot perform a modification on a readonly database in %s on line %d
Insertion failed
Mode parameter is "r":

Warning: dba_insert(): Cannot perform a modification on a readonly database in %s on line %d
Insertion failed
Mode parameter is "wl":
This is a test insert
Mode parameter is "wd":
This is a test insert
Mode parameter is "w-":
This is a test insert
Mode parameter is "w":
This is a test insert
Mode parameter is "cl":
This is a test insert
Mode parameter is "cd":
This is a test insert
Mode parameter is "c-":
This is a test insert
Mode parameter is "c":
This is a test insert
Mode parameter is "nl":
This is a test insert
Mode parameter is "nd":
This is a test insert
Mode parameter is "n-":
This is a test insert
Mode parameter is "n":
This is a test insert
=== OPENING EXISTING RANDOM FILE ===
Mode parameter is "rl":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "rd":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "r-":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "r":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "wl":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "wd":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "w-":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "w":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "cl":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "cd":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "c-":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "c":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "nl":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "nd":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "n-":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
Mode parameter is "n":

Warning: dba_open(): Driver initialization failed for handler: tcadb in %s on line %d
Opening DB failed
