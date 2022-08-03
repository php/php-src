--TEST--
ftp_connect - return FALSE if connection fails and Waning is generated
--EXTENSIONS--
ftp
--FILE--
<?php
$ftp = ftp_connect('dummy-host-name', 21, 5);
var_dump($ftp);
?>
--EXPECTF--
Warning: ftp_connect(): php_network_getaddresses: getaddrinfo for dummy-host-name failed: %s in %s on line %d
bool(false)
