--TEST--
filter_require_var() and FILTER_VALIDATE_REGEXP
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
try {
    var_dump(filter_require_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/.*/'))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(filter_require_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/^b(.*)/'))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(filter_require_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/^d(.*)/'))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(filter_require_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/blah/'))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(filter_require_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/\[/'))));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    var_dump(filter_require_var("data", FILTER_VALIDATE_REGEXP));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECTF--
string(4) "data"
string(107) "Regexp validation: Failed to match (invalid_key: N/A, filter_name: validate_regexp, filter_flags: 33554432)"
string(4) "data"
string(107) "Regexp validation: Failed to match (invalid_key: N/A, filter_name: validate_regexp, filter_flags: 33554432)"
string(107) "Regexp validation: Failed to match (invalid_key: N/A, filter_name: validate_regexp, filter_flags: 33554432)"

Warning: filter_require_var(): 'regexp' option missing in %s on line 28
string(115) "Regexp validation: Missing 'regexp' option (invalid_key: N/A, filter_name: validate_regexp, filter_flags: 33554432)"
Done
