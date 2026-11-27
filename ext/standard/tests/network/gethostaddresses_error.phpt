--TEST--
gethostaddresses() with an unresolvable hostname
--FILE--
<?php
var_dump(gethostaddresses("gethostaddresses.invalid"));
?>
--EXPECTF--
Warning: gethostaddresses(): php_network_getaddresses: getaddrinfo for gethostaddresses.invalid failed: %s in %s on line %d
bool(false)
