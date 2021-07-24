--TEST--
Bug #81286: PDO return phantom rowset and no error when MYSQL_ATTR_USE_BUFFERED_QUERY=FALSE
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require_once __DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc';
require_once __DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc';

$pdo = MySQLPDOTest::factory();
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$pdo->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);

$qr = $pdo->query('SELECT IF(1, (SELECT 1 UNION ALL SELECT 2), 0)');

var_dump($pdo->errorInfo());

if ($qr) {
    var_dump($qr->errorInfo());
    $qr->nextRowset();
    var_dump($qr->errorInfo());
}

?>
--EXPECT--
array(3) {
  [0]=>
  string(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
array(3) {
  [0]=>
  string(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
array(3) {
  [0]=>
  string(5) "21000"
  [1]=>
  int(1242)
  [2]=>
  string(32) "Subquery returns more than 1 row"
}
