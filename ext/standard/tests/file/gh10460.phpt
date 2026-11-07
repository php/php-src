--TEST--
GH-10460 (allow_url_include warning must be consistent even with path with filters)
--INI--
allow_url_fopen=1
allow_url_include=0
--FILE--
<?php
include 'php://filter/read=string.toupper/resource=data://text/plain,<?php echo "a";';
?>
--EXPECTF--
Warning: include(): data:// wrapper is disabled in the server configuration by allow_url_include=0 in %s on line %d

Warning: include(): Failed to open stream: operation failed in %s on line %d

Warning: include(): Failed opening 'php://filter/read=string.toupper/resource=data://text/plain,<?php echo "a";' for inclusion (include_path='%s') in %s on line %d
