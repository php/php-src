--TEST--
Bug #32223 (weird behaviour of pg_last_notice)
--SKIPIF--
<?php 
require_once('skipif.inc');

_skip_lc_messages();
	
@pg_query($conn, "CREATE LANGUAGE 'plpgsql' HANDLER plpgsql_call_handler LANCOMPILER 'PL/pgSQL'");
$res = @pg_query($conn, "CREATE OR REPLACE FUNCTION test_notice() RETURNS boolean AS '
begin
        RAISE NOTICE ''11111'';
        return ''f'';
end;
' LANGUAGE plpgsql;");
if (!$res) die('skip PLPGSQL not available');
?>
--FILE--
<?php

require_once('config.inc');
require_once('lcmess.inc');
	
$dbh = @pg_connect($conn_str);
if (!$dbh) {
	die ("Could not connect to the server");
}

_set_lc_messages();

$res = pg_query($dbh, "CREATE OR REPLACE FUNCTION test_notice() RETURNS boolean AS '
begin
        RAISE NOTICE ''11111'';
        return ''f'';
end;
' LANGUAGE plpgsql;");


$res = pg_query($dbh, 'SELECT test_notice()');
$row = pg_fetch_row($res, 0);
var_dump($row);
pg_free_result($res);
if ($row[0] == 'f')
{
	var_dump(pg_last_notice($dbh));
}

pg_close($dbh);

?>
===DONE===
--EXPECTF--
array(1) {
  [0]=>
  string(1) "f"
}
string(14) "NOTICE:  11111"
===DONE===
