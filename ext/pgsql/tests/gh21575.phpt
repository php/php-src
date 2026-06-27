--TEST--
GH-21575 - Memory leak with notices in pgsql persistent connections.
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");

_skip_lc_messages($conn);

$res = @pg_query($conn, "CREATE OR REPLACE FUNCTION test_notice() RETURNS boolean AS '
begin
        RAISE NOTICE ''test notice'';
        return ''t'';
end;
' LANGUAGE plpgsql;");
if (!$res) die('skip PLPGSQL not available');
?>
--INI--
pgsql.ignore_notice=0
--FILE--
<?php
include('inc/config.inc');
include('inc/lcmess.inc');

$db = pg_pconnect($conn_str);
_set_lc_messages($db);

pg_query($db, "CREATE OR REPLACE FUNCTION test_notice() RETURNS boolean AS '
begin
        RAISE NOTICE ''test notice'';
        return ''t'';
end;
' LANGUAGE plpgsql;");

/* Trigger a notice so the notice handler writes into the link handle. */
pg_query($db, 'SET client_min_messages TO NOTICE;');
pg_query($db, 'SELECT test_notice()');
var_dump(pg_last_notice($db));
pg_close($db);

/* Reconnect with PGSQL_CONNECT_FORCE_NEW which calls PQreset().
 * Before the fix, notices emitted during PQreset (e.g. collation version
 * mismatch) would write into the stale (freed) link handle, causing a
 * memory leak. We cannot reliably trigger such notices in a portable test,
 * but this exercises the persistent reconnect + notice handling path. */
$db2 = pg_pconnect($conn_str, PGSQL_CONNECT_FORCE_NEW);
var_dump($db2 instanceof PgSql\Connection);
pg_query($db2, 'SELECT test_notice()');
var_dump(pg_last_notice($db2));

pg_close($db2);

echo "Done\n";
?>
--CLEAN--
<?php
include('inc/config.inc');
$db = pg_connect($conn_str);
pg_query($db, 'DROP FUNCTION IF EXISTS test_notice()');
pg_close($db);
?>
--EXPECTF--
string(%d) "NOTICE:  test notice"
bool(true)
string(%d) "NOTICE:  test notice"
Done
