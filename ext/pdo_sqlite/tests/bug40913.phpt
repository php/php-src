--TEST--
Bug #40913 (PDO::PARAM_LOB does not bind to a stream for fetching a BLOB)
--SKIPIF--
<?php if (!extension_loaded('pdo_sqlite')) print 'skip not loaded'; ?>
--FILE--
<?php

$pdo = new PDO('sqlite::memory:');
$pdo->exec('CREATE TABLE test (a INTEGER, b BLOB);');
$pdo->exec('INSERT INTO test VALUES (42, zeroblob(5000));');

$stmt = $pdo->prepare('SELECT a, b FROM test WHERE a = 42;');
$stmt->bindColumn(1, $id, PDO::PARAM_INT);
$stmt->bindColumn(2, $blob, PDO::PARAM_LOB);
$stmt->execute();
$stmt->fetch(PDO::FETCH_BOUND);

var_dump($id);
var_dump(is_resource($blob));
var_dump(stream_get_meta_data($blob));
var_dump(strlen(fread($blob, 6000)));

// Try to seek and see if it works
var_dump(fseek($blob, 1000));
var_dump(strlen(fread($blob, 6000)));

// Close
var_dump(fclose($blob));

?>
--EXPECTF--
int(42)
bool(true)
array(7) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
  ["stream_type"]=>
  string(6) "MEMORY"
  ["mode"]=>
  string(2) "rb"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(true)
}
int(5000)
int(0)
int(4000)
bool(true)