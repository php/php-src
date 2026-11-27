--TEST--
GH-25825: pg_meta_data() numeric column names are not integer keys with opcache enabled
--EXTENSIONS--
pgsql
opcache
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
include('inc/config.inc');
$table = 'pg_meta_data_numeric_column';

function check(PgSql\Connection $db, string $table): void {
    foreach (pg_meta_data($db, $table) as $name => $meta) {
        var_dump($name, is_int($name));
    }
}

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table}");
pg_query($db, "CREATE TABLE {$table} (\"123\" integer)");
check($db, $table);
?>
--CLEAN--
<?php
include('inc/config.inc');
$db = pg_connect($conn_str);
pg_query($db, 'DROP TABLE IF EXISTS pg_meta_data_numeric_column');
?>
--EXPECT--
int(123)
bool(true)
