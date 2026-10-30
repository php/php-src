--TEST--
Bug #61411 (PDO Segfaults with PERSISTENT == TRUE && EMULATE_PREPARES == FALSE)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';

$attr = PDO_MYSQL_TEST_ATTR;
if (!$attr) {
    $attr = array();
} else {
    $attr = unserialize($attr);
}
$attr[PDO::ATTR_PERSISTENT] = true;
$attr[PDO::ATTR_EMULATE_PREPARES] = false;
$attr[PDO::ATTR_STRINGIFY_FETCHES] = true;

$db = MySQLPDOTest::factoryWithAttr($attr);

$stmt = $db->prepare("SELECT 1");
$stmt->execute();

foreach ($stmt as $line) {
    var_dump($line);
}

print "done!";
?>
--EXPECT--
array(2) {
  [1]=>
  string(1) "1"
  [0]=>
  string(1) "1"
}
done!
