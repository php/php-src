--TEST--
stream_context_set_option() on a context-less stream must not leak into the default context
--FILE--
<?php
$a = fopen('php://memory', 'r+');
stream_context_set_option($a, 'http', 'filename', 'test.txt');

// The default context must stay untouched.
var_dump(stream_context_get_options(stream_context_get_default()));

// A later context-less stream must not inherit the option.
$b = fopen('php://memory', 'r+');
var_dump(stream_context_get_options($b));

// The stream the option was set on keeps it for itself.
var_dump(stream_context_get_options($a));
?>
--EXPECT--
array(0) {
}
array(0) {
}
array(1) {
  ["http"]=>
  array(1) {
    ["filename"]=>
    string(8) "test.txt"
  }
}
