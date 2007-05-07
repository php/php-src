--TEST--
Bug #14383 (using postgres with DBA causes DBA not to be able to find any keys)
--SKIPIF--
<?php 
require_once(dirname(__FILE__).'/../../dba/tests/skipif.inc');
require_once('skipif.inc');
?>
--FILE--
<?php
require_once('config.inc');
	
$dbh = @pg_connect($conn_str);
if (!$dbh) {
	die ("Could not connect to the server");
}
pg_close($dbh);

require_once(dirname(__FILE__).'/../../dba/tests/test.inc');
require_once(dirname(__FILE__).'/../../dba/tests/dba_handler.inc');

?>
--EXPECTF--
database handler: %s
3NYNYY
Content String 2
Content 2 replaced
Read during write:%sallowed
Content 2 replaced 2nd time
The 6th value
array(3) {
  ["key number 6"]=>
  string(13) "The 6th value"
  ["key2"]=>
  string(27) "Content 2 replaced 2nd time"
  ["key5"]=>
  string(23) "The last content string"
}
