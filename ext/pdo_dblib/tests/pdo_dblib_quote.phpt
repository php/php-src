--TEST--
PDO_DBLIB: Ensure quote function returns expected results
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
getDbConnection();
?>
--FILE--
<?php
require __DIR__ . '/config.inc';
$db = getDbConnection();
var_dump($db->quote(42, PDO::PARAM_INT));
var_dump($db->quote('\'', PDO::PARAM_STR));
var_dump($db->quote('foo', PDO::PARAM_STR));
var_dump($db->quote('foo', PDO::PARAM_STR | PDO::PARAM_STR_CHAR));
var_dump($db->quote('über', PDO::PARAM_STR | PDO::PARAM_STR_NATL));

var_dump($db->getAttribute(PDO::ATTR_DEFAULT_STR_PARAM) === PDO::PARAM_STR_CHAR);
$db->setAttribute(PDO::ATTR_DEFAULT_STR_PARAM, PDO::PARAM_STR_NATL);
var_dump($db->getAttribute(PDO::ATTR_DEFAULT_STR_PARAM) === PDO::PARAM_STR_NATL);

var_dump($db->quote('foo', PDO::PARAM_STR | PDO::PARAM_STR_CHAR));
var_dump($db->quote('über', PDO::PARAM_STR));
var_dump($db->quote('über', PDO::PARAM_STR | PDO::PARAM_STR_NATL));

$db = getDbConnection(PDO::class, [PDO::ATTR_DEFAULT_STR_PARAM => PDO::PARAM_STR_NATL]);
var_dump($db->getAttribute(PDO::ATTR_DEFAULT_STR_PARAM) === PDO::PARAM_STR_NATL);

?>
--EXPECT--
string(4) "'42'"
string(4) "''''"
string(5) "'foo'"
string(5) "'foo'"
string(8) "N'über'"
bool(true)
bool(true)
string(5) "'foo'"
string(8) "N'über'"
string(8) "N'über'"
bool(true)
