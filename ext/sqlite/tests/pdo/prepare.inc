<?php

require_once('connection.inc');

$SQL = array();

$DB = new pdo($CONNECTION);

$DB->setAttribute(PDO_ATTR_ERRMODE, PDO_ERRMODE_WARNING);

?>
