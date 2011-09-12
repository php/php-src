<?php

// copy variables from connect.inc into mysql default connection ini settings, so that implicit mysql_connect() behaviour can be tested where needed
// must be loaded AFTER connect.inc

ini_set('mysql.default_host',		$host);
ini_set('mysql.default_user',		$user);
ini_set('mysql.default_password',	$passwd);

?>
