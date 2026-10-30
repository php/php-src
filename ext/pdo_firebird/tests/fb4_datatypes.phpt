--TEST--
PDO_Firebird: Supported Firebird 4.0 datatypes
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); 
if (Pdo\Firebird::getApiVersion() < 40) {
	die('skip: Firebird API version must be greater than or equal to 40');
}
require 'testdb.inc';
$dbh = getDbConnection();
$stmt = $dbh->query("SELECT RDB\$get_context('SYSTEM', 'ENGINE_VERSION') AS VERSION FROM RDB\$DATABASE");
$data = $stmt->fetch(\PDO::FETCH_ASSOC);
if (!$data || !array_key_exists('VERSION', $data) || version_compare($data['VERSION'], '4.0.0') < 0) {
	die("skip Firebird Server version must be greater than or equal to 4.0.0");
}
?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require 'testdb.inc';

$sql = <<<'SQL'
	SELECT
		CAST(15 AS BIGINT) AS i64,
		CAST(15 AS INT128) AS i128,
		123.97 AS N,
		CAST(123.97 AS NUMERIC(38,2)) AS N2,
		CAST('2024-05-04 12:59:34.239' AS TIMESTAMP) AS TS,
		CAST('2024-05-04 12:59:34.239 Europe/Moscow' AS TIMESTAMP WITH TIME ZONE) AS TS_TZ,
		CAST('12:59:34.239' AS TIME) AS T,
		CAST('12:59:34.239 Europe/Moscow' AS TIME WITH TIME ZONE) AS T_TZ,
		CAST(1.128 AS DECFLOAT(16)) AS df16,
		CAST(1.128 AS DECFLOAT(34)) AS df34
	FROM RDB$DATABASE
SQL;

$dbh = getDbConnection();

$stmt = $dbh->prepare($sql);
$stmt->execute();
$data = $stmt->fetch(\PDO::FETCH_ASSOC);
$stmt->closeCursor();
$str = json_encode($data, JSON_PRETTY_PRINT);
echo $str;
echo "\ndone\n";
?>
--EXPECTF--
{
    "I64": %r(15|"15")%r,
    "I128": "15",
    "N": "123.97",
    "N2": "123.97",
    "TS": "2024-05-04 12:59:34",
    "TS_TZ": "2024-05-04 12:59:34 Europe\/Moscow",
    "T": "12:59:34",
    "T_TZ": "12:59:34 Europe\/Moscow",
    "DF16": "1.128",
    "DF34": "1.128"
}
done
