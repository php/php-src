--TEST--
pg_meta_data() - cache behavior and invalidation
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include('inc/config.inc');

$conn = pg_connect($conn_str);
if (!$conn) die("Connection failed\n");

pg_query($conn, "DROP TABLE IF EXISTS test_meta_cache");
pg_query($conn, "CREATE TABLE test_meta_cache (id SERIAL PRIMARY KEY, name TEXT)");
$meta1 = pg_meta_data($conn, 'test_meta_cache');
if (!isset($meta1['id']) || !isset($meta1['name'])) {
    echo "FAIL: Basic metadata missing columns\n";
    var_dump(array_keys($meta1));
} else {
    echo "Basic metadata: OK (columns: " . implode(', ', array_keys($meta1)) . ")\n";
}

pg_query($conn, "ALTER TABLE test_meta_cache ADD COLUMN age INT");
$meta2 = pg_meta_data($conn, 'test_meta_cache');
if (!isset($meta2['age'])) {
    echo "FAIL: Column 'age' not seen after ALTER\n";
    var_dump(array_keys($meta2));
} else {
    echo "ALTER invalidation: OK ('age' found)\n";
}

pg_query($conn, "DROP TABLE test_meta_cache");
pg_query($conn, "CREATE TABLE test_meta_cache (only_column TEXT)");
$meta3 = pg_meta_data($conn, 'test_meta_cache');
if (isset($meta3['id']) || isset($meta3['name']) || isset($meta3['age'])) {
    echo "FAIL: Old columns still present after DROP/CREATE\n";
    var_dump(array_keys($meta3));
} elseif (!isset($meta3['only_column'])) {
    echo "FAIL: New column 'only_column' missing\n";
    var_dump(array_keys($meta3));
} else {
    echo "DROP/CREATE invalidation: OK (only 'only_column' present)\n";
}

$meta_ext = pg_meta_data($conn, 'test_meta_cache', true);
if (!isset($meta_ext['only_column']['is enum']) || !isset($meta_ext['only_column']['description'])) {
    echo "FAIL: Extended metadata missing extra fields\n";
    var_dump($meta_ext['only_column']);
} else {
    echo "Extended metadata: OK (has 'is enum', 'description')\n";
}

$meta_schema = pg_meta_data($conn, 'public.test_meta_cache');
if (!isset($meta_schema['only_column'])) {
    echo "FAIL: Schema-qualified name not recognized\n";
} else {
    echo "Schema-qualified name: OK";
}
$meta_false = pg_meta_data($conn, 'non_existent_table');
pg_query($conn, "DROP TABLE test_meta_cache");
pg_close($conn);
echo "Done\n";
?>
--CLEAN--
<?php
include('inc/config.inc');
$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS test_meta_cache CASCADE");
?>
--EXPECTF--
Basic metadata: OK (columns: %s)
ALTER invalidation: OK ('age' found)
DROP/CREATE invalidation: OK (only 'only_column' present)
Extended metadata: OK (has 'is enum', 'description')
Schema-qualified name: OK
Warning: pg_meta_data(): Table 'non_existent_table' doesn't exists in %s on line %d
Done