<?php

require_once('connection.inc');

$SQL = array();

try {
	$DB = new pdo($CONNECTION);
} catch (Exception $tmp) {
	$DB = NULL;
	return;
}

foreach(array('test','classtypes') as $name)
{
	$DB->query('DROP TABLE '.$name.' CASCADE');
}

$DB->setAttribute(PDO_ATTR_ERRMODE, PDO_ERRMODE_WARNING);

?>
