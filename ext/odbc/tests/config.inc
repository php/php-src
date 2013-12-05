<?php

putenv('ODBCINI=/etc/odbc.ini');
putenv('ODBCSYSINI=/etc');

$dsn = 'Driver={Microsoft Access Driver (*.mdb, *.accdb)};Dbq=d:\php_test.accdb;';
$user = '';
$pass = '';
