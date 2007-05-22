--TEST--
import_request_variables() test (numeric keys, different order)
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

import_request_variables("gcp", "_");
var_dump($_1, $_2, $_3, $_4, $_5);

echo "Done\n";
?>
--EXPECTF--
string(1) "5"
string(1) "6"
string(1) "7"
string(1) "8"
string(1) "9"
Done
