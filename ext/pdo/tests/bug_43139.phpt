--TEST--
PDO Common: Bug #43139 (PDO ignore ATTR_DEFAULT_FETCH_MODE in some cases with fetchAll())
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

switch ($db->getAttribute(PDO::ATTR_DRIVER_NAME)) {
    case 'oci': $from = ' FROM DUAL'; break;
    case 'firebird': $from = ' FROM RDB$DATABASE'; break;
    default: $from = ''; break;
}

var_dump($db->query("select 0 as abc, 1 as xyz, 2 as def $from")->fetchAll(PDO::FETCH_GROUP));
?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(2) {
      ["xyz"]=>
      string(1) "1"
      ["def"]=>
      string(1) "2"
    }
  }
}
