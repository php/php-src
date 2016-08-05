--TEST--
filter_assert() and FILTER_VALIDATE_REGEXP
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
try {
    var_dump(filter_assert("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/.*/'))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(filter_assert("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/^b(.*)/'))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(filter_assert("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/^d(.*)/'))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(filter_assert("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/blah/'))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(filter_assert("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/\[/'))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(filter_assert("data", FILTER_VALIDATE_REGEXP));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECTF--
string(4) "data"
string(34) "Regexp validation: Failed to match"
string(4) "data"
string(34) "Regexp validation: Failed to match"
string(34) "Regexp validation: Failed to match"

Warning: filter_assert(): 'regexp' option missing in %s on line %d
string(42) "Regexp validation: Missing 'regexp' option"
Done
