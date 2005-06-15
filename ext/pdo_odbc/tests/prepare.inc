<?php

require_once('connection.inc');

$SQL = array(
	'create'=>'CREATE TABLE test(id int NOT NULL PRIMARY KEY, val VARCHAR(10))'
);

//$DB = new pdo($CONNECTION, $USER, $PASSWD);
$DB = new pdo($CONNECTION);

foreach(array('test','classtypes') as $name)
{
	$DB->exec("DROP TABLE $name");
}

$DB->setAttribute(PDO_ATTR_ERRMODE, PDO_ERRMODE_WARNING);

?>
