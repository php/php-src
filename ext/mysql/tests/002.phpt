--TEST--
mysql_fetch_array
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'connect.inc';

$db = mysql_connect($host, $user, $passwd);

var_dump($db);

var_dump(mysql_select_db('test'));

var_dump(mysql_query('DROP TABLE IF EXISTS test'));

var_dump(mysql_query('CREATE TABLE test(col1 int PRIMARY KEY, col2 varchar(50), col3 char(5))'));

var_dump(mysql_query("INSERT INTO test(col1, col2, col3) VALUES(1, 'foo', 'bar'),(2, 'foo', 'bar')"));

var_dump($res = mysql_query('SELECT col1, col2, col3 FROM test ORDER BY col1 ASC'));

while ($data = mysql_fetch_array($res, MYSQL_ASSOC)) {
	var_dump($data);
}

mysql_close($db);

?>
--EXPECTF--
resource(%d) of type (mysql link)
bool(true)
bool(true)
bool(true)
bool(true)
resource(%d) of type (mysql result)
array(3) {
  ["col1"]=>
  string(1) "1"
  ["col2"]=>
  string(3) "foo"
  ["col3"]=>
  string(3) "bar"
}
array(3) {
  ["col1"]=>
  string(1) "2"
  ["col2"]=>
  string(3) "foo"
  ["col3"]=>
  string(3) "bar"
}
