--TEST--
CURLOPT_POSTFIELDS with nested arrays
--EXTENSIONS--
curl
--FILE--
<?php

$fields = [
	'single' => 'SingleValue',
	'multi' => [
		['array', 'invalid' => 'value'],
		'Multi2',
	]
];

$options = [
	CURLOPT_POST => 1,
	CURLOPT_HEADER => 0,
	CURLOPT_FRESH_CONNECT => 1,
	CURLOPT_RETURNTRANSFER => 1,
	CURLOPT_FORBID_REUSE => 1,
	CURLOPT_TIMEOUT => 1,
	CURLOPT_POSTFIELDS => $fields,
	CURLOPT_HTTPHEADER => [ 'Expect:' ],
];

try {
    $ch = curl_init();
    curl_setopt_array($ch, $options);
} catch (TypeError $e) {
    echo "TypeError: {$e->getMessage()}\n";
}
var_dump(curl_errno($ch) === CURLE_BAD_FUNCTION_ARGUMENT);
?>
--EXPECT--
TypeError: curl_setopt_array(): Nested arrays are not supported for key multi
bool(true)
