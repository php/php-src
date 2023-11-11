--TEST--
DBA LMDB handler flags test
--EXTENSIONS--
dba
--SKIPIF--
<?php
    $handler = 'lmdb';
    require_once __DIR__ .'/skipif.inc';
?>
--FILE--
<?php
$handler = 'lmdb';

// Pass bogus flag
try {
    $db_file = dba_open('irrelevant', 'c', $handler, flags: 45);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

// Use current test folder
$db_filename = __DIR__;
$db_file = dba_open($db_filename, 'c', $handler, flags: DBA_LMDB_USE_SUB_DIR);
assert($db_file !== false);

// Check insertion of data
dba_insert("key1", "Content String 1", $db_file);
dba_insert("key2", "Content String 2", $db_file);
dba_insert("key3", "Third Content String", $db_file);
dba_insert("key4", "Another Content String", $db_file);
dba_insert("key5", "The last content string", $db_file);

// Remove some data
dba_delete("key3", $db_file);
dba_delete("key1", $db_file);

// Fetch data
$key = dba_firstkey($db_file);
$total_keys = 0;
while ($key) {
    echo $key, ': ', dba_fetch($key, $db_file), \PHP_EOL;
    $key = dba_nextkey($db_file);
    $total_keys++;
}
echo 'Total keys: ', $total_keys, \PHP_EOL;
for ($i = 1; $i < 6; $i++) {
    echo "Key $i exists? ", dba_exists("key$i", $db_file) ? "Y" : "N", \PHP_EOL;
}

// Replace second key data
dba_replace("key2", "Content 2 replaced", $db_file);
echo dba_fetch("key2", $db_file), \PHP_EOL;

// Close handler
dba_close($db_file);

?>
--CLEAN--
<?php
$db_filename = __DIR__ . '/data.mdb';
$db_loc_filename = __DIR__ . '/lock.mdb';
@unlink($db_filename);
@unlink($db_loc_filename);
?>
--EXPECT--
dba_open(): Argument #6 ($flags) must be either DBA_LMDB_USE_SUB_DIR or DBA_LMDB_NO_SUB_DIR for LMDB driver
key2: Content String 2
key4: Another Content String
key5: The last content string
Total keys: 3
Key 1 exists? N
Key 2 exists? Y
Key 3 exists? N
Key 4 exists? Y
Key 5 exists? Y
Content 2 replaced
