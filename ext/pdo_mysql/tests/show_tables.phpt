--TEST--
PDO MySQL SHOW TABLES
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo_mysql')) print 'skip not loaded';
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_mysql/tests/common.phpt');

print_r($db->query('SHOW TABLES'));
--EXPECT--
PDOStatement Object
(
    [queryString] => SHOW TABLES
)
