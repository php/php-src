<?php

if (!extension_loaded('odbc')) die('skip odbc extension not loaded');

include 'config.inc';

$conn = @odbc_connect($dsn, $user, $pass);
if (!$conn) {
	die('skip could not connect');
}
