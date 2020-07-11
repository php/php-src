--TEST--
Test ftp_ssl_connect() function : error conditions
--SKIPIF--
<?php
$ssl = 1;
require 'skipif.inc';
if (!function_exists("ftp_ssl_connect")) die("skip ftp_ssl is disabled");
?>
--FILE--
<?php
echo "*** Testing ftp_ssl_connect() function : error conditions ***\n";
echo "\n-- Testing ftp_ssl_connect() function on failure --\n";
var_dump(ftp_ssl_connect('totes.invalid'));

echo "\n-- Testing ftp_ssl_connect() function timeout warning for value 0 --\n";
ftp_ssl_connect('totes.invalid', 21, 0);

echo "===DONE===\n";
--EXPECTF--
*** Testing ftp_ssl_connect() function : error conditions ***

-- Testing ftp_ssl_connect() function on failure --

Warning: ftp_ssl_connect(): php_network_getaddresses: getaddrinfo failed: %s in %s on line %d
bool(false)

-- Testing ftp_ssl_connect() function timeout warning for value 0 --

Warning: ftp_ssl_connect(): Timeout has to be greater than 0 in %s on line %d
===DONE===
