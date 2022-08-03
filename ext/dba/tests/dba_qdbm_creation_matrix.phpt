--TEST--
DBA QDBM opening matrix of combination
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('qdbm');
?>
--FILE--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';

$handler = 'qdbm';
run_creation_tests($handler);

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$handler = 'qdbm';
clean_creation_tests($handler);
?>
--EXPECTF--
=== OPENING NON-EXISTING FILE ===
Mode parameter is "rl":

Notice: dba_open(): Handler qdbm does locking internally in %s on line %d

Warning: dba_open(): Driver initialization failed for handler: qdbm: open error in %s on line %d
Opening DB failed
Mode parameter is "rd":

Warning: dba_open(): Driver initialization failed for handler: qdbm: open error in %s on line %d
Opening DB failed
Mode parameter is "r-":

Warning: dba_open(): Locking cannot be disabled for handler qdbm in %s on line %d
Opening DB failed
Mode parameter is "r":

Warning: dba_open(): Driver initialization failed for handler: qdbm: open error in %s on line %d
Opening DB failed
Mode parameter is "wl":

Notice: dba_open(): Handler qdbm does locking internally in %s on line %d

Warning: dba_open(): Driver initialization failed for handler: qdbm: open error in %s on line %d
Opening DB failed
Mode parameter is "wd":

Warning: dba_open(): Driver initialization failed for handler: qdbm: open error in %s on line %d
Opening DB failed
Mode parameter is "w-":

Warning: dba_open(): Locking cannot be disabled for handler qdbm in %s on line %d
Opening DB failed
Mode parameter is "w":

Warning: dba_open(): Driver initialization failed for handler: qdbm: open error in %s on line %d
Opening DB failed
Mode parameter is "cl":

Notice: dba_open(): Handler qdbm does locking internally in %s on line %d
This is a test insert
Mode parameter is "cd":
This is a test insert
Mode parameter is "c-":

Warning: dba_open(): Locking cannot be disabled for handler qdbm in %s on line %d
Opening DB failed
Mode parameter is "c":
This is a test insert
Mode parameter is "nl":

Notice: dba_open(): Handler qdbm does locking internally in %s on line %d
This is a test insert
Mode parameter is "nd":
This is a test insert
Mode parameter is "n-":

Warning: dba_open(): Locking cannot be disabled for handler qdbm in %s on line %d
Opening DB failed
Mode parameter is "n":
This is a test insert
=== OPENING EXISTING DB FILE ===
Mode parameter is "rl":

Notice: dba_open(): Handler qdbm does locking internally in %s on line %d

Warning: dba_insert(): Cannot perform a modification on a readonly database in %s on line %d
Insertion failed
Mode parameter is "rd":

Warning: dba_insert(): Cannot perform a modification on a readonly database in %s on line %d
Insertion failed
Mode parameter is "r-":

Warning: dba_open(): Locking cannot be disabled for handler qdbm in %s on line %d
Opening DB failed
Mode parameter is "r":

Warning: dba_insert(): Cannot perform a modification on a readonly database in %s on line %d
Insertion failed
Mode parameter is "wl":

Notice: dba_open(): Handler qdbm does locking internally in %s on line %d
This is a test insert
Mode parameter is "wd":
This is a test insert
Mode parameter is "w-":

Warning: dba_open(): Locking cannot be disabled for handler qdbm in %s on line %d
Opening DB failed
Mode parameter is "w":
This is a test insert
Mode parameter is "cl":

Notice: dba_open(): Handler qdbm does locking internally in %s on line %d
This is a test insert
Mode parameter is "cd":
This is a test insert
Mode parameter is "c-":

Warning: dba_open(): Locking cannot be disabled for handler qdbm in %s on line %d
Opening DB failed
Mode parameter is "c":
This is a test insert
Mode parameter is "nl":

Notice: dba_open(): Handler qdbm does locking internally in %s on line %d
This is a test insert
Mode parameter is "nd":
This is a test insert
Mode parameter is "n-":

Warning: dba_open(): Locking cannot be disabled for handler qdbm in %s on line %d
Opening DB failed
Mode parameter is "n":
This is a test insert
=== OPENING EXISTING RANDOM FILE ===
Mode parameter is "rl":

Notice: dba_open(): Handler qdbm does locking internally in %s on line %d

Warning: dba_open(): Driver initialization failed for handler: qdbm: broken database file in %s on line %d
Opening DB failed
Mode parameter is "rd":

Warning: dba_open(): Driver initialization failed for handler: qdbm: broken database file in %s on line %d
Opening DB failed
Mode parameter is "r-":

Warning: dba_open(): Locking cannot be disabled for handler qdbm in %s on line %d
Opening DB failed
Mode parameter is "r":

Warning: dba_open(): Driver initialization failed for handler: qdbm: broken database file in %s on line %d
Opening DB failed
Mode parameter is "wl":

Notice: dba_open(): Handler qdbm does locking internally in %s on line %d

Warning: dba_open(): Driver initialization failed for handler: qdbm: broken database file in %s on line %d
Opening DB failed
Mode parameter is "wd":

Warning: dba_open(): Driver initialization failed for handler: qdbm: broken database file in %s on line %d
Opening DB failed
Mode parameter is "w-":

Warning: dba_open(): Locking cannot be disabled for handler qdbm in %s on line %d
Opening DB failed
Mode parameter is "w":

Warning: dba_open(): Driver initialization failed for handler: qdbm: broken database file in %s on line %d
Opening DB failed
Mode parameter is "cl":

Notice: dba_open(): Handler qdbm does locking internally in %s on line %d

Warning: dba_open(): Driver initialization failed for handler: qdbm: broken database file in %s on line %d
Opening DB failed
Mode parameter is "cd":

Warning: dba_open(): Driver initialization failed for handler: qdbm: broken database file in %s on line %d
Opening DB failed
Mode parameter is "c-":

Warning: dba_open(): Locking cannot be disabled for handler qdbm in %s on line %d
Opening DB failed
Mode parameter is "c":

Warning: dba_open(): Driver initialization failed for handler: qdbm: broken database file in %s on line %d
Opening DB failed
Mode parameter is "nl":

Notice: dba_open(): Handler qdbm does locking internally in %s on line %d
This is a test insert
Mode parameter is "nd":
This is a test insert
Mode parameter is "n-":

Warning: dba_open(): Locking cannot be disabled for handler qdbm in %s on line %d
Opening DB failed
Mode parameter is "n":
This is a test insert
