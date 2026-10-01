--TEST--
http_build_query() function with object that is just stringable (GH-10229)
--FILE--
<?php
class StringableObject {
    public function __toString() : string {
        return "Stringable";
    }
}

$o = new StringableObject();

var_dump(http_build_query(['hello', $o]));
var_dump(http_build_query($o));
var_dump(http_build_query(['hello', $o], numeric_prefix: 'prefix_'));
var_dump(http_build_query($o, numeric_prefix: 'prefix_'));
?>
--EXPECTF--
Deprecated: http_build_query(): object values within argument #1 $data to http_build_query() being interpreted as arrays is deprecated, instead the $data argument should be preprocessed with get_object_vars() in %s on line %d
string(7) "0=hello"

Deprecated: http_build_query(): Passing an object for argument #1 $data to http_build_query() is deprecated, call get_object_vars() first instead in %s on line %d
string(0) ""

Deprecated: http_build_query(): object values within argument #1 $data to http_build_query() being interpreted as arrays is deprecated, instead the $data argument should be preprocessed with get_object_vars() in %s on line %d
string(14) "prefix_0=hello"

Deprecated: http_build_query(): Passing an object for argument #1 $data to http_build_query() is deprecated, call get_object_vars() first instead in %s on line %d
string(0) ""
