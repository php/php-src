<?php

require_once('connection.inc');

$SQL = array();

$DB = new pdo($CONNECTION, $USER, $PASSWD);

foreach(array('test','classtypes') as $name)
{
	$DB->query('DROP TABLE IF EXISTS '.$name);
}

?>