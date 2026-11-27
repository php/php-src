--TEST--
GH-25802: pg_get_notify() result is never === false with opcache enabled
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

function check(PgSql\Connection $db): void {
    $msg = pg_get_notify($db);
    var_dump($msg === false);
}

$db = pg_connect($conn_str);
check($db);
pg_query($db, 'LISTEN pg_get_notify_false');
pg_query($db, 'NOTIFY pg_get_notify_false');
check($db);
?>
--EXPECT--
bool(true)
bool(false)
