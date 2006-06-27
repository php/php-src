<?php

include 'connect.inc';
if (!extension_loaded("mysql")) {
    die('skip mysql extension not available');
}
$link = @mysql_connect($host, $user, $passwd);
if (!$link) die('skip cannot connect');
mysql_close($link);

?>
