<?php

include 'connect.inc';
$link = @mysql_connect($host, $user, $passwd);
if (!$link) die('skip cannot connect');
mysql_close($link);

?>