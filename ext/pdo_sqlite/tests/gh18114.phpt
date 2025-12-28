--TEST--
GH-18114 (pdo lazy object crash)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = new PDO('sqlite::memory:');
$x = $db->query('select 1 as queryString');
$data = $x->fetch(PDO::FETCH_LAZY);
foreach ($data as $entry) {
    var_dump($entry);
}
var_dump((array) $data);
echo "Done\n";
?>
--EXPECT--
array(0) {
}
Done
