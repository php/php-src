<?php

require_once('connection.inc');

$SQL = array();

$DB = new pdo($CONNECTION);

foreach(array('test','classtypes') as $name)
{
	$DB->query('DROP TABLE '.$name.' CASCADE');
}

$DB->setAttribute(PDO_ATTR_ERRMODE, PDO_ERRMODE_WARNING);

?>
