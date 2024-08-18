--TEST--
PDO_Firebird: Firebird 4.0 set session time zone
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); 
if (Pdo\Firebird::getApiVersion() < 40) {
	die('skip: Firebird API version must be greater than or equal to 40');
}
require 'testdb.inc';
checkMinServerVersion('4.0.0');
?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php

$sql = <<<'SQL'
	SELECT 
		rdb$get_context('SYSTEM', 'SESSION_TIMEZONE') as tz,
		trim(replace(current_timestamp, localtimestamp, '')) as tz1,
		trim(replace(current_time, localtime, '')) as tz2
	FROM rdb$database
SQL;

$dbh = connectToDb([
    Pdo\Firebird::SESSION_TIMEZONE => 'Europe/Rome',
]);

$stmt = $dbh->prepare($sql);
$stmt->execute();
$data = $stmt->fetch(\PDO::FETCH_ASSOC);
$stmt->closeCursor();

var_dump($data);
echo "\ndone\n";
?>
--EXPECT--
array(3) {
  ["TZ"]=>
  string(11) "Europe/Rome"
  ["TZ1"]=>
  string(11) "Europe/Rome"
  ["TZ2"]=>
  string(11) "Europe/Rome"
}

done
