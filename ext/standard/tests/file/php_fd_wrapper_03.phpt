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

Warning: fopen(): failed to open stream (php://fd): operation failed in %s on line 2

Warning: fopen(): failed to open stream (php://fd/): php://fd/ stream must be specified in the form php://fd/<orig fd> in %s on line %d

Warning: fopen(): failed to open stream (php://fd/-2): The file descriptors must be non-negative numbers smaller than %d in %s on line %d

Warning: fopen(): failed to open stream (php://fd/1/): php://fd/ stream must be specified in the form php://fd/<orig fd> in %s on line %d

Done.
