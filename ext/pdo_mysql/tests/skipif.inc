<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql'))
	die('skip PDO_MySQL driver not loaded');

if (version_compare(PHP_VERSION, '5.1.0') < 0)
	die('skip Most tests assume PHP 5.1+');
?>