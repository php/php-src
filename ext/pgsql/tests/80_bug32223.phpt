--TEST--
Bug #27597 pg_fetch_array not returning false 
--SKIPIF--
<?php 
require_once('skipif.inc');
?>
--FILE--
<?php

require_once('config.inc');
	
$dbh = @pg_connect($conn_str);
if (!$dbh) {
	die ("Could not connect to the server");
}

pg_query($dbh, "CREATE OR REPLACE FUNCTION test_notice() RETURNS boolean AS '
begin
        RAISE NOTICE ''11111'';
        return ''f'';
end;
' LANGUAGE plpgsql;");

try
{
	$res = pg_query($dbh, 'SELECT test_notice()');
	$row = pg_fetch_row($res, 0);
	var_dump($row);
	pg_free_result($res);
	if ($row[0] == 'f')
	{
		throw new Exception(pg_last_notice($dbh));
	}
	echo 2;
}
catch (Exception $e)
{
	var_dump($e->getMessage());
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
