--TEST--
php://fd wrapper: bad syntax
--FILE--
<?php
fopen("php://fd", "w");
fopen("php://fd/", "w");
fopen("php://fd/-2", "w");
fopen("php://fd/1/", "w");

echo "\nDone.\n";
?>
--EXPECTF--
Warning: fopen('php://fd', 'w'): Invalid php:// URL specified in %s on line %d

Warning: fopen('php://fd', 'w'): Failed to open stream: operation failed in %s on line %d

Warning: fopen('php://fd/', 'w'): Failed to open stream: php://fd/ stream must be specified in the form php://fd/<orig fd> in %s on line %d

Warning: fopen('php://fd/-2', 'w'): Failed to open stream: The file descriptors must be non-negative numbers smaller than 256 in %s on line %d

Warning: fopen('php://fd/1/', 'w'): Failed to open stream: php://fd/ stream must be specified in the form php://fd/<orig fd> in %s on line %d

Done.
