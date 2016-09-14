--TEST--
PDO_DBLIB: Uniqueidentifier column data type stringifying
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require __DIR__ . '/config.inc';
if (in_array(get_tds_version(), ['4.2', '4.6'])) die('skip feature unsupported by this TDS version');
?>
--FILE--
<?php
require __DIR__ . '/config.inc';


$testGUID = '82A88958-672B-4C22-842F-216E2B88E72A';
$testGUIDBinary = base64_decode('WImogitnIkyELyFuK4jnKg==');

$sql = "SELECT CAST('$testGUID' as uniqueidentifier) as [guid]";

//--------------------------------------------------------------------------------
// 1. Get and Set the attribute
//--------------------------------------------------------------------------------
$db->setAttribute(PDO::DBLIB_ATTR_STRINGIFY_UNIQUEIDENTIFIER, true);
var_dump(true === $db->getAttribute(PDO::DBLIB_ATTR_STRINGIFY_UNIQUEIDENTIFIER));
$db->setAttribute(PDO::DBLIB_ATTR_STRINGIFY_UNIQUEIDENTIFIER, false);
var_dump(false === $db->getAttribute(PDO::DBLIB_ATTR_STRINGIFY_UNIQUEIDENTIFIER));


//--------------------------------------------------------------------------------
// 2. Binary
//--------------------------------------------------------------------------------
$stmt = $db->query($sql);
$row = $stmt->fetch(PDO::FETCH_ASSOC);

var_dump($row['guid'] === $testGUIDBinary);


//--------------------------------------------------------------------------------
// 3. PDO::ATTR_STRINGIFY_FETCHES must not affect `uniqueidentifier` representation
//--------------------------------------------------------------------------------
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
$stmt = $db->query($sql);
$row = $stmt->fetch(PDO::FETCH_ASSOC);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

var_dump($row['guid'] === $testGUIDBinary);


//--------------------------------------------------------------------------------
// 4. Stringifying
// ! With TDS protocol version <7.0 binary will be returned and the test will fail !
// TODO: something from PDO::ATTR_SERVER_VERSION, PDO::ATTR_CLIENT_VERSION or PDO::ATTR_SERVER_INFO should be used
// to get TDS version and skip this test in this case.
//--------------------------------------------------------------------------------
$db->setAttribute(PDO::DBLIB_ATTR_STRINGIFY_UNIQUEIDENTIFIER, true);
$stmt = $db->query($sql);
$row = $stmt->fetch(PDO::FETCH_ASSOC);

var_dump($row['guid'] === $testGUID);
var_dump($row['guid']);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(36) "82A88958-672B-4C22-842F-216E2B88E72A"
