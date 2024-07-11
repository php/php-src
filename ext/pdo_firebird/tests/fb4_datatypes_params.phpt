--TEST--
PDO_Firebird: Supported Firebird 4.0 datatypes (parameters)
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require 'testdb.inc';

$sql = <<<'SQL'
	SELECT 
		CAST(? AS INT128) AS i128,
		CAST(? AS NUMERIC(38,2)) AS N2,
		CAST(? AS TIMESTAMP WITH TIME ZONE)  AS TS_TZ,
		CAST(? AS TIME WITH TIME ZONE) AS T_TZ,
		CAST(? AS DECFLOAT(16)) AS df16,
		CAST(? AS DECFLOAT(34)) AS df34
	FROM RDB$DATABASE
SQL;

$dbh = getDbConnection();

$stmt = $dbh->prepare($sql);
$stmt->execute([12, 12.34, '2024-05-04 12:59:34.239 Europe/Moscow', '12:59 Europe/Moscow', 12.34, 12.34]);
$data = $stmt->fetch(\PDO::FETCH_ASSOC);
$stmt->closeCursor();
$str = json_encode($data, JSON_PRETTY_PRINT);
echo $str;
echo "\ndone\n";
?>
--EXPECT--
{
    "I128": "12",
    "N2": "12.34",
    "TS_TZ": "2024-05-04 12:59:34.2390 Europe\/Moscow",
    "T_TZ": "12:59:00.0000 Europe\/Moscow",
    "DF16": "12.34",
    "DF34": "12.34"
}
done
