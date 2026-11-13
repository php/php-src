--TEST--
PDO_sqlite: PDO::FETCH_OBJ with duplicate, numeric and empty column names
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = new PDO('sqlite::memory:');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$row = $db->query('SELECT 1 AS a, 2 AS a, 3 AS "0", 4 AS "01", 5 AS "", 6 AS "-1"')->fetch(PDO::FETCH_OBJ);
var_dump($row);
var_dump($row->{'0'}, (array) $row);

$row->a = 'changed';
$row->extra = true;
unset($row->{'01'});
var_dump($row);

var_dump($db->query('SELECT 1 AS g, 2 AS v UNION ALL SELECT 1, 3')->fetchAll(PDO::FETCH_OBJ | PDO::FETCH_GROUP));
?>
--EXPECTF--
object(stdClass)#%d (5) {
  ["a"]=>
  int(2)
  ["0"]=>
  int(3)
  ["01"]=>
  int(4)
  [""]=>
  int(5)
  ["-1"]=>
  int(6)
}
int(3)
array(5) {
  ["a"]=>
  int(2)
  [0]=>
  int(3)
  ["01"]=>
  int(4)
  [""]=>
  int(5)
  [-1]=>
  int(6)
}
object(stdClass)#%d (5) {
  ["a"]=>
  string(7) "changed"
  ["0"]=>
  int(3)
  [""]=>
  int(5)
  ["-1"]=>
  int(6)
  ["extra"]=>
  bool(true)
}
array(1) {
  [1]=>
  array(2) {
    [0]=>
    object(stdClass)#%d (1) {
      ["v"]=>
      int(2)
    }
    [1]=>
    object(stdClass)#%d (1) {
      ["v"]=>
      int(3)
    }
  }
}
