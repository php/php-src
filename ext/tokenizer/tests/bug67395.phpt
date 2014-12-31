--TEST--
Bug 67395: token_name() does not return name for T_POW and T_POW_EQUAL token
--FILE--
<?php

$powToken = token_get_all('<?php **')[1][0];
var_dump(token_name($powToken));

$powEqualToken = token_get_all('<?php **=')[1][0];
var_dump(token_name($powEqualToken));

?>
--EXPECT--
string(5) "T_POW"
string(11) "T_POW_EQUAL"
