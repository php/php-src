--TEST--
gethostbyname() function - basic invalid parameter test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--FILE--
<?php
    var_dump(gethostbyname(".toto.toto.toto"));
?>
--EXPECTF--
Warning: gethostbyname(): php_network_getaddresses: getaddrinfo for .toto.toto.toto failed: %s in %s on line %d
string(15) ".toto.toto.toto"
