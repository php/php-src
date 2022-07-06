--TEST--
PDO Common: PDOStatement::debugDumpParams() with emulated prepares
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();

$db = PDOTest::factory();
if ($db->getAttribute(PDO::ATTR_DRIVER_NAME) == 'pgsql') die('skip pgsql has its own test for this feature');
if (!@$db->getAttribute(PDO::ATTR_EMULATE_PREPARES) && !@$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true)) die('skip driver cannot emulate prepared statements');
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

$db = PDOTest::factory();
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);

$stmt = $db->query('SELECT 1');

// "Sent SQL" shouldn't display
var_dump($stmt->debugDumpParams());

$stmt = $db->prepare('SELECT :bool, :int, :string, :null');
$stmt->bindValue(':bool', true, PDO::PARAM_BOOL);
$stmt->bindValue(':int', 123, PDO::PARAM_INT);
$stmt->bindValue(':string', 'foo', PDO::PARAM_STR);
$stmt->bindValue(':null', null, PDO::PARAM_NULL);
$stmt->execute();

// "Sent SQL" should display
var_dump($stmt->debugDumpParams());

?>
--EXPECT--
SQL: [8] SELECT 1
Params:  0
NULL
SQL: [34] SELECT :bool, :int, :string, :null
Sent SQL: [26] SELECT 1, 123, 'foo', NULL
Params:  4
Key: Name: [5] :bool
paramno=-1
name=[5] ":bool"
is_param=1
param_type=5
Key: Name: [4] :int
paramno=-1
name=[4] ":int"
is_param=1
param_type=1
Key: Name: [7] :string
paramno=-1
name=[7] ":string"
is_param=1
param_type=2
Key: Name: [5] :null
paramno=-1
name=[5] ":null"
is_param=1
param_type=0
NULL
