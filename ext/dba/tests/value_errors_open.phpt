--TEST--
DBA argument tests
--EXTENSIONS--
dba
--CONFLICTS--
dba
--SKIPIF--
<?php
require_once(__DIR__ .'/skipif.inc');
die("info $HND handler used");
?>
--FILE--
<?php
require_once(__DIR__ .'/test.inc');
echo "database handler: $handler\n";
var_dump(dba_open($db_file, 'n'));

echo '=== Invalid arguments dba_open() ===', \PHP_EOL;
var_dump(dba_open($db_file, 'n', 'bogus'));

try {
    var_dump(dba_open('', 'nq'));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(dba_open($db_file, ''));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(dba_open($db_file, 'nq', ''));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}


try {
    var_dump(dba_open($db_file, 'q'));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(dba_open($db_file, 'nq'));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(dba_open($db_file, 'rdq'));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(dba_open($db_file, 'n-t'));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

try {
    var_dump(dba_open($db_file, 'r', $handler, 0o644, -10));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo '=== Invalid arguments dba_popen() ===', \PHP_EOL;
var_dump(dba_popen($db_file, 'n', 'bogus'));

try {
    var_dump(dba_popen('', 'nq'));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(dba_popen($db_file, ''));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(dba_popen($db_file, 'nq', ''));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}


try {
    var_dump(dba_popen($db_file, 'q'));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(dba_popen($db_file, 'nq'));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(dba_popen($db_file, 'rdq'));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(dba_popen($db_file, 'n-t'));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

try {
    var_dump(dba_popen($db_file, 'r', $handler, 0o644, -10));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
?>
--CLEAN--
<?php
require(__DIR__ .'/clean.inc');
?>
--EXPECTF--
database handler: flatfile
resource(%d) of type (dba)
=== Invalid arguments dba_open() ===

Warning: dba_open(): Handler "bogus" is not available in %s on line %d
bool(false)
dba_open(): Argument #1 ($path) cannot be empty
dba_open(): Argument #2 ($mode) cannot be empty
dba_open(): Argument #3 ($handler) cannot be empty
dba_open(): Argument #2 ($mode) first character must be one of "r", "w", "c", or "n"
dba_open(): Argument #2 ($mode) second character must be one of "d", "l", "-", or "t"
dba_open(): Argument #2 ($mode) third character must be "t"
dba_open(): Argument #2 ($mode) cannot combine mode "-" (no lock) and "t" (test lock)
dba_open(): Argument #5 ($map_size) must be greater than or equal to 0
=== Invalid arguments dba_popen() ===

Warning: dba_popen(): Handler "bogus" is not available in %s on line %d
bool(false)
dba_popen(): Argument #1 ($path) cannot be empty
dba_popen(): Argument #2 ($mode) cannot be empty
dba_popen(): Argument #3 ($handler) cannot be empty
dba_popen(): Argument #2 ($mode) first character must be one of "r", "w", "c", or "n"
dba_popen(): Argument #2 ($mode) second character must be one of "d", "l", "-", or "t"
dba_popen(): Argument #2 ($mode) third character must be "t"
dba_popen(): Argument #2 ($mode) cannot combine mode "-" (no lock) and "t" (test lock)
dba_popen(): Argument #5 ($map_size) must be greater than or equal to 0
