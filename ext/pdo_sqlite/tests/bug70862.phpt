--TEST--
PDO_sqlite: Testing sqliteCreateCollation()
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PDO('sqlite::memory:');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->exec('CREATE TABLE test_70862(field BLOB)');

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

class HelloWrapper {
    public $context;
    public function stream_open() { return true; }
    public function stream_eof() { return true; }
    public function stream_read() { return NULL; }
    public function stream_stat() { return array(); }
}
stream_wrapper_register("hello", "HelloWrapper");

$f = fopen("hello://there", "r");

$stmt = $db->prepare('INSERT INTO test_70862(field) VALUES (:para)');
$stmt->bindParam(":para", $f, PDO::PARAM_LOB);
$stmt->execute();

var_dump($f);

?>
+++DONE+++
--EXPECT--
string(0) ""
+++DONE+++
