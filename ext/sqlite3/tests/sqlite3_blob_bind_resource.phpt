--TEST--
SQLite3::execute() with a resource bound for blob param
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/new_db.inc');
require_once(dirname(__FILE__) . '/stream_test.inc');

var_dump($db->exec('CREATE TABLE test (id STRING, data BLOB)'));
$insert_stmt = $db->prepare("INSERT INTO test (id, data) VALUES (1, ?)");


class HelloWrapper {
	public function stream_open() { return true; }
	public function stream_eof() { return true; }
	public function stream_read() { return NULL; }
	public function stream_stat() { return array(); }
}
stream_wrapper_register("hello", "HelloWrapper");

$f = fopen("hello://there", "r");

var_dump($insert_stmt->bindParam(1, $f, SQLITE3_BLOB));
var_dump($insert_stmt->execute());

var_dump($insert_stmt->close());
fclose($f);

?>
+++DONE+++
--EXPECTF--
bool(true)
bool(true)
object(SQLite3Result)#%d (%d) {
}
bool(true)
+++DONE+++
