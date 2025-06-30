--TEST--
SQLite3Result::fetchAll usage
--EXTENSIONS--
sqlite3
--FILE--
<?php
$conn = new sqlite3(':memory:');
$conn->query('CREATE TABLE users (id INTEGER NOT NULL, num INTEGER NOT NULL, PRIMARY KEY(id))');

$stmt = $conn->query('insert into users (id, num) values (1, 1)');
$stmt = $conn->query('insert into users (id, num) values (2, 2)');

$stmt = $conn->query('SELECT * FROM users');
$rowall = $stmt->fetchAll();
var_dump($rowall);
$stmt->reset();
$rowfetch = [];
while (($row = $stmt->fetchArray())) $rowfetch[] = $row;
var_dump($rowfetch);
var_dump($rowall == $rowfetch);
$stmt->reset();
var_dump($stmt->fetchAll(SQLITE3_NUM));
$stmt->reset();
var_dump($stmt->fetchAll(SQLITE3_ASSOC));

?>
--EXPECT--
array(2) {
  [0]=>
  array(4) {
    [0]=>
    int(1)
    ["id"]=>
    int(1)
    [1]=>
    int(1)
    ["num"]=>
    int(1)
  }
  [1]=>
  array(4) {
    [0]=>
    int(2)
    ["id"]=>
    int(2)
    [1]=>
    int(2)
    ["num"]=>
    int(2)
  }
}
array(2) {
  [0]=>
  array(4) {
    [0]=>
    int(1)
    ["id"]=>
    int(1)
    [1]=>
    int(1)
    ["num"]=>
    int(1)
  }
  [1]=>
  array(4) {
    [0]=>
    int(2)
    ["id"]=>
    int(2)
    [1]=>
    int(2)
    ["num"]=>
    int(2)
  }
}
bool(true)
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(1)
  }
  [1]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(2)
  }
}
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    int(1)
    ["num"]=>
    int(1)
  }
  [1]=>
  array(2) {
    ["id"]=>
    int(2)
    ["num"]=>
    int(2)
  }
}
