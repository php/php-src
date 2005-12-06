--TEST--
filter_data() & FL_IP and weird data
--GET--

--FILE--
<?php

var_dump(filter_data("....", FL_IP));
var_dump(filter_data("...", FL_IP));
var_dump(filter_data("..", FL_IP));
var_dump(filter_data(".", FL_IP));
var_dump(filter_data("1.1.1.1", FL_IP));

echo "Done\n";
?>
--EXPECTF--	
NULL
NULL
NULL
NULL
string(7) "1.1.1.1"
Done
