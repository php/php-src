--TEST--
gethostbyname() function - basic invalid parameter test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--FILE--
<?php
    var_dump(gethostbyname(".toto.toto.toto"));
?>
--EXPECT--
string(15) ".toto.toto.toto"
