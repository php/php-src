--TEST--
PDO PgSQL Bug #62593 (Emulate prepares behave strangely with PARAM_BOOL)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$errors = array();

$query = $db->prepare('SELECT :foo IS FALSE as val_is_false');
$query->bindValue(':foo', true, PDO::PARAM_BOOL);
$query->execute();
$errors[] = $query->errorInfo();

$query->bindValue(':foo', 0, PDO::PARAM_BOOL);
$query->execute();
$errors[] = $query->errorInfo();

$query->bindValue(':foo', false, PDO::PARAM_BOOL);
$query->execute();
$errors[] = $query->errorInfo();

$expect = 'No errors found';

foreach ($errors as $error)
{
  if (strpos('Invalid text representation', $error[2]) !== false)
  {
    $expect = 'Invalid boolean found';
  }
}
echo $expect;
?>
--EXPECTF--

No errors found
