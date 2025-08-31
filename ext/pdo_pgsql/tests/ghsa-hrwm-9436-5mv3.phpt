--TEST--
#GHSA-hrwm-9436-5mv3: pdo_pgsql extension does not check for errors during escaping
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

$invalid = "ABC\xff\x30';";
var_dump($db->quote($invalid));

?>
--EXPECT--
bool(false)
