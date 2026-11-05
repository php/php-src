--TEST--
crypt() function - characters > 0x80
--FILE--
<?php
var_dump(crypt("À1234abcd", "99"));
var_dump(crypt("À9234abcd", "99"));
var_dump(crypt("À1234abcd", "_J9..4567"));
var_dump(crypt("À9234abcd", "_J9..4567"));
?>
--EXPECT--
string(13) "99PxawtsTfX56"
string(13) "99jcVcGxUZOWk"
string(20) "_J9..4567q0YG9xIr3M6"
string(20) "_J9..4567xcl/AKtT5rI"
