--TEST--
Basic test for stream_context_set_options()
--FILE--
<?php
$context = stream_context_create();

$options = [
    'http' => [
        'protocol_version' => 1.1,
        'user_agent' => 'PHPT Agent',
    ],
];
var_dump(stream_context_set_options($context, $options));

var_dump(stream_context_get_options($context));
?>
--EXPECT--
bool(true)
array(1) {
  ["http"]=>
  array(2) {
    ["protocol_version"]=>
    float(1.1)
    ["user_agent"]=>
    string(10) "PHPT Agent"
  }
}
