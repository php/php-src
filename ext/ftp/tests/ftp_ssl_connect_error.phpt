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

echo "\n-- Testing ftp_ssl_connect() function invalid argument type --\n";
ftp_ssl_connect([]);
ftp_ssl_connect('totes.invalid', []);
ftp_ssl_connect('totes.invalid', 21, []);

echo "\n-- Testing ftp_ssl_connect() function with more than expected no. of arguments --\n";
ftp_ssl_connect('totes.invalid', 21, 1, []);

echo "\n-- Testing ftp_ssl_connect() function timeout warning for value 0 --\n";
ftp_ssl_connect('totes.invalid', 21, 0);

echo "===DONE===\n";
--EXPECTF--
*** Testing ftp_ssl_connect() function : error conditions ***

-- Testing ftp_ssl_connect() function on failure --

Warning: ftp_ssl_connect(): php_network_getaddresses: getaddrinfo failed: %s in %s on line %d
bool(false)

-- Testing ftp_ssl_connect() function invalid argument type --

Warning: ftp_ssl_connect() expects parameter 1 to be string, array given in %s on line %d

Warning: ftp_ssl_connect() expects parameter 2 to be int, array given in %s on line %d

Warning: ftp_ssl_connect() expects parameter 3 to be int, array given in %s on line %d

-- Testing ftp_ssl_connect() function with more than expected no. of arguments --

Warning: ftp_ssl_connect() expects at most 3 parameters, 4 given in %s on line %d

-- Testing ftp_ssl_connect() function timeout warning for value 0 --

Warning: ftp_ssl_connect(): Timeout has to be greater than 0 in %s on line %d
===DONE===
