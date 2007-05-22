--TEST--
import_request_variables() test (numeric keys)
--GET--
1=0&2=1&3=2&4=3&5=4
--POST--
1=5&2=6&3=7&4=8&5=9
--COOKIE--
1=10;2=11;3=12;4=13;5=14
--INI--
variables_order=CGP
--FILE--
<?php

import_request_variables("gpc", "_");
var_dump($_1, $_2, $_3, $_4, $_5);

echo "Done\n";
?>
--EXPECTF--
string(2) "10"
string(2) "11"
string(2) "12"
string(2) "13"
string(2) "14"
Done
