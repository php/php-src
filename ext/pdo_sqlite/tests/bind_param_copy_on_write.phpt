--TEST--
PDO SQLite keeps the bound value when a PHP string changes during fetch
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = new PDO('sqlite::memory:');
foreach ([PDO::PARAM_STR, PDO::PARAM_LOB] as $type) {
    $value = str_repeat('a', random_int(3, 3));
    $stmt = $db->prepare('SELECT :v UNION ALL SELECT :v');
    $stmt->bindParam(':v', $value, $type);
    $stmt->execute();
    var_dump($stmt->fetchColumn());
    $value[0] = 'b';
    var_dump($stmt->fetchColumn(), $value);
}
?>
--EXPECT--
string(3) "aaa"
string(3) "aaa"
string(3) "baa"
string(3) "aaa"
string(3) "aaa"
string(3) "baa"
