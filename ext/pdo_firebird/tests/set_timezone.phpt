--TEST--
PDO_Firebird: Set session timezone in connection properties
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); 
if (Pdo\Firebird::getApiVersion() < 40) {
	die('skip: Firebird API version must be greater than or equal to 40');
}
require_once 'testdb.inc';
$dbh = getDbConnection();
$stmt = $dbh->query("SELECT RDB\$get_context('SYSTEM', 'ENGINE_VERSION') AS VERSION FROM RDB\$DATABASE");
$data = $stmt->fetch(\PDO::FETCH_ASSOC);
if (!$data || !array_key_exists('VERSION', $data) || version_compare($data['VERSION'], '4.0.0') < 0) {
	die("skip Firebird Server version must be greater than or equal to 4.0.0");
}
unset($stmt);
unset($dbh);
?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require_once 'testdb.inc';

$sql = <<<'SQL'
SELECT 
  rdb$get_context('SYSTEM', 'SESSION_TIMEZONE') as tz,
  trim(replace(current_timestamp, localtimestamp, '')) as tz1,
  trim(replace(current_time, localtime, '')) as tz2
FROM rdb$database
SQL;

$attr = [
      Pdo\Firebird::SESSION_TIMEZONE => 'Europe/Rome'
];

$dbh = connectToDb($attr);

$stmt = $dbh->prepare($sql);
$stmt->execute();
$data = $stmt->fetch(\PDO::FETCH_ASSOC);
$stmt->closeCursor();
$str = json_encode($data, JSON_PRETTY_PRINT);
echo $str;
echo "\ndone\n";
?>
--EXPECT--
{
    "TZ": "Europe\/Rome",
    "TZ1": "Europe\/Rome",
    "TZ2": "Europe\/Rome"
}
done
