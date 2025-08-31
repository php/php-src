--TEST--
filter_var() and FILTER_VALIDATE_REGEXP
--EXTENSIONS--
filter
--FILE--
<?php

var_dump(filter_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/.*/'))));
var_dump(filter_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/^b(.*)/'))));
var_dump(filter_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/^d(.*)/'))));
var_dump(filter_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/blah/'))));
var_dump(filter_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/\[/'))));
try {
    filter_var("data", FILTER_VALIDATE_REGEXP);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECT--
string(4) "data"
bool(false)
string(4) "data"
bool(false)
bool(false)
filter_var(): "regexp" option is missing
Done
