--TEST--
DBA CDB opening matrix of combination
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('cdb');
?>
--FILE--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';

$handler = 'cdb';
run_creation_tests($handler);

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$handler = 'cdb';
clean_creation_tests($handler);
?>
--EXPECTF--
=== OPENING NON-EXISTING FILE ===
Mode parameter is "rl":

Warning: dba_open(cdb_not_existing.db): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "rd":

Warning: dba_open(cdb_not_existing.db): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "r-":

Warning: dba_open(cdb_not_existing.db): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "r":

Warning: dba_open(cdb_not_existing.db): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "wl":

Warning: dba_open(cdb_not_existing.db): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "wd":

Warning: dba_open(cdb_not_existing.db): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "w-":

Warning: dba_open(cdb_not_existing.db): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "w":

Warning: dba_open(cdb_not_existing.db): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "cl":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "cd":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "c-":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "c":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "nl":
Cannot fetch insertion
Mode parameter is "nd":
Cannot fetch insertion
Mode parameter is "n-":
Cannot fetch insertion
Mode parameter is "n":
Cannot fetch insertion
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

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "wd":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "w-":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "w":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "cl":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "cd":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "c-":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "c":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "nl":
Cannot fetch insertion
Mode parameter is "nd":
Cannot fetch insertion
Mode parameter is "n-":
Cannot fetch insertion
Mode parameter is "n":
Cannot fetch insertion
=== OPENING EXISTING RANDOM FILE ===
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

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "wd":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "w-":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "w":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "cl":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "cd":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "c-":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "c":

Warning: dba_open(): Driver initialization failed for handler: cdb: Update operations are not supported in %s on line %d
Opening DB failed
Mode parameter is "nl":
Cannot fetch insertion
Mode parameter is "nd":
Cannot fetch insertion
Mode parameter is "n-":
Cannot fetch insertion
Mode parameter is "n":
Cannot fetch insertion
