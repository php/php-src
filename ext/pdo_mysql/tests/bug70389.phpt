--TEST--
Bug #70389 (PDO constructor changes unrelated variables)
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require(__DIR__. DIRECTORY_SEPARATOR . 'config.inc');
$flags = [
    PDO::MYSQL_ATTR_FOUND_ROWS	=> true,
    PDO::MYSQL_ATTR_LOCAL_INFILE	=> true,
    PDO::ATTR_PERSISTENT 		=> true,
];

$std = new StdClass();
$std->flags = $flags;

new PDO(PDO_MYSQL_TEST_DSN, PDO_MYSQL_TEST_USER, PDO_MYSQL_TEST_PASS, $flags);
var_dump($flags);

?>
--EXPECTF--
array(3) {
  [%d]=>
  bool(true)
  [1001]=>
  bool(true)
  [12]=>
  bool(true)
}
