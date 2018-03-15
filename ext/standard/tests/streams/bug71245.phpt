--TEST--
Bug #71245 (file_get_contents() ignores "header" context option if it's a reference)
--FILE--
<?php
$headers = ['Host: okey.com'];
$httpContext = [
	'http' => [
		'protocol_version'	=> '1.1',
		'method'			=> 'GET',
		'header'			=> &$headers,
		'follow_location'	=> 0,
		'max_redirects'		=> 0,
		'ignore_errors'		=> true,
		'timeout'			=> 60,
	],
];
$context = stream_context_create($httpContext);
$headers = ["Host: bad.com"];
print_r(stream_context_get_options($context));
?>
--EXPECT--
Array
(
    [http] => Array
        (
            [protocol_version] => 1.1
            [method] => GET
            [header] => Array
                (
                    [0] => Host: okey.com
                )

            [follow_location] => 0
            [max_redirects] => 0
            [ignore_errors] => 1
            [timeout] => 60
        )

)
