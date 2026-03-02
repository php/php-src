--TEST--
stream_context_set_option() function - basic test for stream_context_set_option()
--CREDITS--
Jean-Marc Fontaine <jean-marc.fontaine@alterway.fr>
# Alter Way Contribution Day 2011
--FILE--
<?php
$context = stream_context_create();

// Single option
var_dump(stream_context_set_option($context, 'http', 'method', 'POST'));

// Array of options
$options = array(
    'http' => array(
        'protocol_version' => 1.1,
        'user_agent'       => 'PHPT Agent',
    ),
);
var_dump(stream_context_set_option($context, $options));

var_dump(stream_context_get_options($context));
?>
--EXPECTF--
bool(true)

Deprecated: Calling stream_context_set_option() with 2 arguments is deprecated, use stream_context_set_options() instead in %s on line %d
bool(true)
array(1) {
  ["http"]=>
  array(3) {
    ["method"]=>
    string(4) "POST"
    ["protocol_version"]=>
    float(1.1)
    ["user_agent"]=>
    string(10) "PHPT Agent"
  }
}
