<?php

include 'connect.inc';
$link = @mysql_connect($host, $user, $passwd);
if (!$link) die('skip canot connect');
mysql_close($link);

?>