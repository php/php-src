--TEST--
filter_data() and FL_REGEXP
--GET--

--FILE--
<?php

var_dump(filter_data("data", FL_REGEXP, array("regexp"=>'/.*/')));
var_dump(filter_data("data", FL_REGEXP, array("regexp"=>'/^b(.*)/')));
var_dump(filter_data("data", FL_REGEXP, array("regexp"=>'/^d(.*)/')));
var_dump(filter_data("data", FL_REGEXP, array("regexp"=>'blah')));
var_dump(filter_data("data", FL_REGEXP, array("regexp"=>'[')));
var_dump(filter_data("data", FL_REGEXP));

echo "Done\n";
?>
--EXPECTF--	
string(4) "data"
NULL
string(4) "data"
NULL
NULL
PHP Warning:  filter_data(): 'regexp' option missing in %s on line %d

Warning: filter_data(): 'regexp' option missing in %s on line %d
NULL
Done
