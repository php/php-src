--TEST--
PDO Common: Explicit disconnect of common and persistent PDO
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();

$db = PDOTest::factory();
try {
  $db->beginTransaction();
} catch (PDOException $exception) {
  die("skip not relevant for driver - does not permit transactions");
}
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

$db1 = PDOTest::factory('PDO', false);
var_dump($db1->isConnected());
$db2 = PDOTest::factory('PDO', false);
var_dump($db2->isConnected());
putenv("PDOTEST_ATTR=" . serialize(array(PDO::ATTR_PERSISTENT => true)));
$pdb3 = PDOTest::factory('PDO', false);
var_dump($pdb3->isConnected());
$pdb4 = PDOTest::factory('PDO', false);
$pdb4->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
var_dump($pdb4->isConnected());

/* disconnect regular PDO, confirm other PDOs still connected */
var_dump($db1->disconnect());
var_dump($db1->isConnected());
var_dump($db1->beginTransaction());
var_dump($db1->errorCode());
var_dump($db1->errorInfo());
var_dump($db2->isConnected());
var_dump($pdb3->isConnected());

/* disconnect persistent PDO, confirm other persistent PDO disconnected */
var_dump($pdb3->disconnect());
var_dump($pdb3->isConnected());
var_dump($pdb4->isConnected());
var_dump($pdb4->disconnect());

/* new persistent PDO should prompt new connection */
$pdb5 = PDOTest::factory('PDO', false);
var_dump($pdb5->isConnected());
var_dump($pdb5->inTransaction());
var_dump($pdb5->beginTransaction());

/* new persistent connection should not be inherited */
var_dump($pdb4->isConnected());
try {
	$pdb4->beginTransaction();
} catch (PDOException $e) {
	var_dump($e->getMessage());
}

$db1 = null;
$db2 = null; /* trigger shutdown without explicit disconnect */
$pdb3 = null;
$pdb4 = null;
$pdb5 = null; /* destruct should not disconnect */

/* no PDOs remain, but persistent connection should remain */
$pdb6 = PDOTest::factory('PDO', false);
var_dump($pdb6->inTransaction());

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)

Warning: PDO::beginTransaction(): SQLSTATE[01002]: Disconnect error in %s on line %d
bool(false)
string(5) "01002"
array(3) {
  [0]=>
  string(5) "01002"
  [1]=>
  NULL
  [2]=>
  NULL
}
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
string(33) "SQLSTATE[01002]: Disconnect error"
bool(true)
