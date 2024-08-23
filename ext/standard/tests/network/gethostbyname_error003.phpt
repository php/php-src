--TEST--
gethostbyname() function - basic type return error test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--FILE--
<?php
    var_dump(is_string(gethostbyname("asdfasdf")));
?>
--EXPECTF--
Warning: gethostbyname(): php_network_getaddresses: getaddrinfo for asdfasdf failed: nodename nor servname provided, or not known in %s on line %d
bool(true)
