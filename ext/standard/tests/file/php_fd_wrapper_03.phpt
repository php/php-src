--TEST--
php://fd wrapper: bad syntax
--FILE--
<?php
fopen("php://fd", "w");
fopen("php://fd/", "w");
fopen("php://fd/-2", "w");
fopen("php://fd/1/", "w");

echo "\nDone.\n";
--EXPECTF--
Warning: fopen(): Invalid php:// URL specified in %s on line %d

Warning: fopen(php://fd): failed to open stream: operation failed in %s on line 2

Warning: fopen(php://fd/): failed to open stream: php://fd/ stream must be specified in the form php://fd/<orig fd> in %s on line %d

Warning: fopen(php://fd/-2): failed to open stream: The file descriptors must be non-negative numbers smaller than %d in %s on line %d

Warning: fopen(php://fd/1/): failed to open stream: php://fd/ stream must be specified in the form php://fd/<orig fd> in %s on line %d

Done.
