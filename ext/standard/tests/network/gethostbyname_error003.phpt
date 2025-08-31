--TEST--
gethostbyname() function - basic type return error test
--CREDITS--
"Sylvain R." <sracine@phpquebec.org>
--FILE--
<?php
    var_dump(is_string(gethostbyname("asdfasdf")));
?>
--EXPECT--
bool(true)
