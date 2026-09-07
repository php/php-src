--TEST--
Text column with NUL bytes
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');

$db->exec(
    'CREATE TABLE messages (
        content TEXT
    )'
);

$insert = $db->prepare(
    'INSERT INTO messages (content) VALUES (:content)'
);

$insert->bindValue(':content', "with\0null", SQLITE3_TEXT);
$insert->execute();
$insert->bindValue(':content', "\0", SQLITE3_TEXT);
$insert->execute();

$result = $db->query('SELECT * FROM messages');
while ($row = $result->fetchArray(SQLITE3_ASSOC)) {
    var_dump($row);
}

?>
--EXPECTF--
array(1) {
  ["content"]=>
  string(9) "with%0null"
}
array(1) {
  ["content"]=>
  string(1) "%0"
}
