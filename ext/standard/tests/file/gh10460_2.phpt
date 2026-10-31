--TEST--
GH-10460 (allow_url_fopen warning must be consistent even with a path with filters)
--INI--
allow_url_fopen=0
--FILE--
<?php
var_dump(file_get_contents('php://filter/read=string.toupper/resource=data://text/plain,a'));
?>
--EXPECTF--
Warning: file_get_contents(): data:// wrapper is disabled in the server configuration by allow_url_fopen=0 in %s on line %d

Warning: file_get_contents(): Failed to open stream: operation failed in %s on line %d
bool(false)
