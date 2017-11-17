--TEST--
PDO_DBLIB: Ensure quote function returns expected results
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
var_dump($db->quote(true, PDO::PARAM_BOOL));
var_dump($db->quote(false, PDO::PARAM_BOOL));
var_dump($db->quote(42, PDO::PARAM_INT));
var_dump($db->quote(null, PDO::PARAM_NULL));
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

$db = new PDO($dsn, $user, $pass, [PDO::ATTR_DEFAULT_STR_PARAM => PDO::PARAM_STR_NATL]);
var_dump($db->getAttribute(PDO::ATTR_DEFAULT_STR_PARAM) === PDO::PARAM_STR_NATL);

?>
--EXPECT--
string(3) "'1'"
string(2) "''"
string(4) "'42'"
string(2) "''"
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
