--TEST--
gethostbyname() function - basic type return error test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--FILE--
<?php
    var_dump(is_string(gethostbyname("asdfasdf")));
?>
--EXPECTF--
Warning: gethostbyname(): php_network_getaddresses: getaddrinfo for asdfasdf failed: %s in %s on line %d
bool(true)
