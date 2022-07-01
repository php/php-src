--TEST--
DBA DB4 opening matrix of combination
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('db4');
?>
--FILE--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';

$handler = 'db4';

$modes = ['r', 'w', 'c', 'n'];
$locks = ['l', 'd', '-', '' /* No lock flag is like 'd' */];

echo '=== OPENING NON-EXISTING FILE ===', \PHP_EOL;
/* Trying to open a non-existing file */
$db_name = $handler . '_not_existing.db';
foreach ($modes as $mode) {
    foreach ($locks as $lock) {
        $arg = $mode.$lock;
        echo 'Mode parameter is "', $arg, '":', \PHP_EOL;
        $db = dba_open($db_name, $arg, $handler);
        if ($db !== false) {
            assert(file_exists($db_name));
            $status = dba_insert("key1", "This is a test insert", $db);
            if ($status) {
                echo dba_fetch("key1", $db), \PHP_EOL;
            } else {
                echo 'Insertion failed', \PHP_EOL;
            }
            dba_close($db);
        } else {
            echo 'Opening DB failed', \PHP_EOL;
        }
        cleanup_standard_db($db_name);
    }
}

echo '=== OPENING EXISTING DB FILE ===', \PHP_EOL;
/* Trying to open an existing db file */
$db_name = $handler . '_existing.db';
foreach ($modes as $mode) {
    foreach ($locks as $lock) {
        dba_open($db_name, 'n', $handler);
        $arg = $mode.$lock;
        echo 'Mode parameter is "', $arg, '":', \PHP_EOL;
        $db = dba_open($db_name, $arg, $handler);
        if ($db !== false) {
            assert(file_exists($db_name));
            $status = dba_insert("key1", "This is a test insert", $db);
            if ($status) {
                echo dba_fetch("key1", $db), \PHP_EOL;
            } else {
                echo 'Insertion failed', \PHP_EOL;
            }
            dba_close($db);
        } else {
            echo 'Opening DB failed', \PHP_EOL;
        }
        cleanup_standard_db($db_name);
    }
}

echo '=== OPENING EXISTING RANDOM FILE ===', \PHP_EOL;
/* Trying to open an existing random file */
$db_name = $handler . '_random.txt';
foreach ($modes as $mode) {
    foreach ($locks as $lock) {
        file_put_contents($db_name, "Dummy contents");
        $arg = $mode.$lock;
        echo 'Mode parameter is "', $arg, '":', \PHP_EOL;
        $db = dba_open($db_name, $arg, $handler);
        if ($db !== false) {
            assert(file_exists($db_name));
            $status = dba_insert("key1", "This is a test insert", $db);
            if ($status) {
                echo dba_fetch("key1", $db), \PHP_EOL;
            } else {
                echo 'Insertion failed', \PHP_EOL;
            }
            dba_close($db);
        } else {
            echo 'Opening DB failed', \PHP_EOL;
        }
        cleanup_standard_db($db_name);
    }
}

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$handler = 'db4';
$db_name = $handler . '_not_existing.db';
cleanup_standard_db($db_name);
$db_name = $handler . '_existing.db';
cleanup_standard_db($db_name);
$db_name = $handler . '_random.txt';
cleanup_standard_db($db_name);
?>
--EXPECTF--
=== OPENING NON-EXISTING FILE ===
Mode parameter is "rl":

Warning: dba_open(): Driver initialization failed for handler: db4: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "rd":

Warning: dba_open(db4_not_existing.db): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "r-":

Warning: dba_open(): Driver initialization failed for handler: db4: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "r":

Warning: dba_open(db4_not_existing.db): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "wl":

Warning: dba_open(): Driver initialization failed for handler: db4: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "wd":

Warning: dba_open(db4_not_existing.db): Failed to open stream: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "w-":

Warning: dba_open(): Driver initialization failed for handler: db4: No such file or directory in %s on line %d
Opening DB failed
Mode parameter is "w":

Warning: dba_open(db4_not_existing.db): Failed to open stream: No such file or directory in %s on line %d
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

Warning: dba_insert(): You cannot perform a modification to a database without proper access in %s on line %d
Insertion failed
Mode parameter is "rd":

Warning: dba_insert(): You cannot perform a modification to a database without proper access in %s on line %d
Insertion failed
Mode parameter is "r-":

Warning: dba_insert(): You cannot perform a modification to a database without proper access in %s on line %d
Insertion failed
Mode parameter is "r":

Warning: dba_insert(): You cannot perform a modification to a database without proper access in %s on line %d
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

Warning: dba_open(): Driver initialization failed for handler: db4: Invalid argument in %s on line %d
Opening DB failed
Mode parameter is "rd":

Warning: dba_open(): Driver initialization failed for handler: db4: Invalid argument in %s on line %d
Opening DB failed
Mode parameter is "r-":

Warning: dba_open(): Driver initialization failed for handler: db4: Invalid argument in %s on line %d
Opening DB failed
Mode parameter is "r":

Warning: dba_open(): Driver initialization failed for handler: db4: Invalid argument in %s on line %d
Opening DB failed
Mode parameter is "wl":

Warning: dba_open(): Driver initialization failed for handler: db4: Invalid argument in %s on line %d
Opening DB failed
Mode parameter is "wd":

Warning: dba_open(): Driver initialization failed for handler: db4: Invalid argument in %s on line %d
Opening DB failed
Mode parameter is "w-":

Warning: dba_open(): Driver initialization failed for handler: db4: Invalid argument in %s on line %d
Opening DB failed
Mode parameter is "w":

Warning: dba_open(): Driver initialization failed for handler: db4: Invalid argument in %s on line %d
Opening DB failed
Mode parameter is "cl":

Warning: dba_open(): Driver initialization failed for handler: db4: Invalid argument in %s on line %d
Opening DB failed
Mode parameter is "cd":

Warning: dba_open(): Driver initialization failed for handler: db4: Invalid argument in %s on line %d
Opening DB failed
Mode parameter is "c-":

Warning: dba_open(): Driver initialization failed for handler: db4: Invalid argument in %s on line %d
Opening DB failed
Mode parameter is "c":

Warning: dba_open(): Driver initialization failed for handler: db4: Invalid argument in %s on line %d
Opening DB failed
Mode parameter is "nl":
This is a test insert
Mode parameter is "nd":
This is a test insert
Mode parameter is "n-":
This is a test insert
Mode parameter is "n":
This is a test insert
