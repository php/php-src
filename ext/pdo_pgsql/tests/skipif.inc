<?php

if (!extension_loaded('pdo_pgsql')) die('skip');

require_once('connection.inc');
require_once('prepare.inc');

if (!$DB) {
	die('skip cannot open db');
}

?>
