<?php

if (!extension_loaded('odbc')) die('skip');

include 'config.inc';

$conn = @odbc_connect($dsn, $user, $pass);
if (!$conn) {
	die('skip could not connect');
}
