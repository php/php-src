<?php

if (!extension_loaded('pdo_odbc')) die('skip');

require_once('connection.inc');
require_once('prepare.inc');

if (!$DB) {
	die('skip cannot open db');
}

?>
