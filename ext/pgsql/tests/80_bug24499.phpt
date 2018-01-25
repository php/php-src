--TEST--
Bug #24499 (Notice: Undefined property: stdClass::)
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

@pg_query("DROP SEQUENCE id_id_seq");
@pg_query("DROP TABLE id");
pg_query("CREATE TABLE id (id SERIAL, t INT)");

for ($i=0; $i<4; $i++) {
	pg_query("INSERT INTO id (t) VALUES ($i)");
}

class Id
{
	public $id;

	public function getId()
	{
		global $dbh;

		$q  = pg_query($dbh, "SELECT id FROM id");
		print_r(pg_fetch_array($q));
		print_r(pg_fetch_array($q));
		$id = pg_fetch_object($q);
		var_dump($id);
		return $id->id;
	}
}

$id = new Id();
var_dump($id->getId());

pg_close($dbh);

echo "Done\n";

?>
--EXPECTF--
Array
(
    [0] => 1
    [id] => 1
)
Array
(
    [0] => 2
    [id] => 2
)
object(stdClass)#%d (1) {
  ["id"]=>
  string(1) "3"
}
string(1) "3"
Done
