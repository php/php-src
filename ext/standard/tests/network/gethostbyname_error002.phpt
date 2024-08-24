--TEST--
gethostbyname() function - basic type return error test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--FILE--
<?php
    var_dump(gethostbyname("1234567890"));
?>
--EXPECT--
string(12) "73.150.2.210"
