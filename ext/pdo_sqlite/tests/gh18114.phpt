--TEST--
GH-18114 (pdo lazy object crash)
--EXTENSIONS--
pdo_sqlite
--XLEAK--
See https://github.com/php/php-src/issues/18114#issuecomment-2738069692, will be fixed in a later PR on lower branches
--FILE--
<?php
$db = new PDO('sqlite::memory:');
$x = $db->query('select 1 as queryString');
foreach ($x->fetch(PDO::FETCH_LAZY) as $entry) {
    var_dump($entry);
}
echo "Done\n";
?>
--EXPECT--
Done
