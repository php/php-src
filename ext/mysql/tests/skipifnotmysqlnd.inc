<?php
if (strpos(mysql_get_client_info(), 'mysqlnd') === false) {
	die('skip mysqlnd extension not available');
}
?>
