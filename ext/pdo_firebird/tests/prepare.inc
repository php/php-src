<?php

require_once('connection.inc');

$SQL = array();

$DB = new pdo($CONNECTION, $USER, $PASSWORD);

$DB->query('DROP TABLE test');
$DB->query('DROP TABLE classtypes');

$DB->setAttribute(PDO_ATTR_ERRMODE, PDO_ERRMODE_WARNING);
$DB->setAttribute(PDO_ATTR_CASE, PDO_CASE_LOWER);

?>
