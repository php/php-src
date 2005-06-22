<?php

require_once('connection.inc');

$SQL = array(
	'create'=>'CREATE TABLE test(id int NOT NULL PRIMARY KEY, val VARCHAR(10))'
);

//$DB = new pdo($CONNECTION, $USER, $PASSWD);
try {
	$DB = new pdo($CONNECTION);
} catch (Exception $tmp) {
	$DB = NULL;
	return;
}

foreach(array('test','classtypes') as $name)
{
	$DB->exec("DROP TABLE $name");
}

$DB->setAttribute(PDO_ATTR_ERRMODE, PDO_ERRMODE_WARNING);

?>
