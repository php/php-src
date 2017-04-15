--TEST--
MySQL ensure quote function returns expected results
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();

var_dump($db->quote('foo', PDO::PARAM_STR));
var_dump($db->quote('foo', PDO::PARAM_STR | PDO::PARAM_STR_CHAR));
var_dump($db->quote('über', PDO::PARAM_STR | PDO::PARAM_STR_NATL));

var_dump($db->getAttribute(PDO::ATTR_DEFAULT_STR_PARAM) === PDO::PARAM_STR_CHAR);
$db->setAttribute(PDO::ATTR_DEFAULT_STR_PARAM, PDO::PARAM_STR_NATL);
var_dump($db->getAttribute(PDO::ATTR_DEFAULT_STR_PARAM) === PDO::PARAM_STR_NATL);

var_dump($db->quote('foo', PDO::PARAM_STR | PDO::PARAM_STR_CHAR));
var_dump($db->quote('über', PDO::PARAM_STR));
var_dump($db->quote('über', PDO::PARAM_STR | PDO::PARAM_STR_NATL));
?>
--EXPECT--
string(5) "'foo'"
string(5) "'foo'"
string(8) "N'über'"
bool(true)
bool(true)
string(5) "'foo'"
string(8) "N'über'"
string(8) "N'über'"
