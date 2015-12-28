<?php
$php_openssl_cipher_tests = array(
	'aes-128-gcm' => array(
		array(
			'key' => '00000000000000000000000000000000',
			'iv'  => '000000000000000000000000',
			'tag' => 'ab6e47d42cec13bdf53a67b21257bddf',
			'pt'  => '00000000000000000000000000000000',
			'ct'  => '0388dace60b6a392f328c2b971b2fe78',
		),
	)
);

function openssl_get_cipher_tests($method)
{
	global $php_openssl_cipher_tests;

	$tests = array();

	foreach ($php_openssl_cipher_tests[$method] as $instance) {
		$test = array();
		foreach ($instance as $field_name => $field_value) {
			$test[$field_name] = pack("H*", $field_value);
		}
		if (!isset($test['aad'])) {
			$test['aad'] = "";
		}
		$tests[] = $test;
	}

	return $tests;
}
