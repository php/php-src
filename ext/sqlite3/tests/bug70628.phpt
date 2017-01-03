--TEST--
Bug #70628 (Clearing bindings on an SQLite3 statement doesn't work)
--SKIPIF--
<?php
if (!extension_loaded('sqlite3')) die('skip'); ?>
--FILE--
<?php
$db = new SQLite3(':memory:');

$db->exec("CREATE TABLE Dogs (Id INTEGER PRIMARY KEY, Breed TEXT, Name TEXT, Age INTEGER)");

$sth = $db->prepare("INSERT INTO Dogs (Breed, Name, Age)  VALUES (:breed,:name,:age)");

$sth->bindValue(':breed', 'canis', SQLITE3_TEXT);
$sth->bindValue(':name', 'jack', SQLITE3_TEXT);
$sth->bindValue(':age', 7, SQLITE3_INTEGER);
$sth->execute();

$sth->clear();
$sth->reset();

$sth->bindValue(':breed', 'russel', SQLITE3_TEXT);
$sth->bindValue(':age', 3, SQLITE3_INTEGER);
$sth->execute();

$res = $db->query('SELECT * FROM Dogs');
while (($row = $res->fetchArray(SQLITE3_ASSOC))) {
    var_dump($row);
}
$res->finalize();

$sth->close();
$db->close();
?>
--EXPECT--
array(4) {
  ["Id"]=>
  int(1)
  ["Breed"]=>
  string(5) "canis"
  ["Name"]=>
  string(4) "jack"
  ["Age"]=>
  int(7)
}
array(4) {
  ["Id"]=>
  int(2)
  ["Breed"]=>
  string(6) "russel"
  ["Name"]=>
  NULL
  ["Age"]=>
  int(3)
}
