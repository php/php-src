--TEST--
Bug #32223 (weird behaviour of pg_last_notice using define)
--SKIPIF--
<?php 
require_once('skipif.inc');
	
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

define('dbh', pg_connect($conn_str));
if (!dbh) {
        die ("Could not connect to the server");
}
pg_exec(dbh, "SET LC_MESSAGES='C';");

//@pg_query(dbh, "CREATE LANGUAGE 'plpgsql' HANDLER plpgsql_call_handler LANCOMPILER 'PL/pgSQL'");
$res = pg_query(dbh, "CREATE OR REPLACE FUNCTION test_notice() RETURNS boolean AS '
begin
        RAISE NOTICE ''11111'';
        return ''f'';
end;
' LANGUAGE plpgsql;");

function tester() {
        $res = pg_query(dbh, 'SELECT test_notice()');
        $row = pg_fetch_row($res, 0);
		var_dump($row);
        pg_free_result($res);
        if ($row[0] == 'f')
        {
                var_dump(pg_last_notice(dbh));
        }
}
tester();

pg_close(dbh);

?>
===DONE===
--EXPECTF--
array(1) {
  [0]=>
  string(1) "f"
}
string(14) "NOTICE:  11111"
===DONE===
