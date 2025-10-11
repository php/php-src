--TEST--
#GHSA-8xr5-qppj-gvwj: NULL Pointer Derefernce for failed user input quoting
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require_once dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
require_once dirname(__FILE__) . '/config.inc';
PDOTest::skip();
?>
--FILE--
<?php
require_once dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
require_once dirname(__FILE__) . '/config.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);

$sql = "SELECT * FROM users where username = :username";
$stmt = $db->prepare($sql);

$p1 = "alice\x99";
var_dump($stmt->execute(['username' => $p1]));

?>
--EXPECT--
bool(false)
