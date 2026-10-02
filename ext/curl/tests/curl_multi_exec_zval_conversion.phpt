--TEST--
curl_multi_exec(): zval conversion for still_running
--EXTENSIONS--
curl
--FILE--
<?php

$mh = curl_multi_init();

$values = [
	'1',
	'abc',
	1.5,
	true,
	false,
	null,
	[],
	new stdClass(),
];

foreach ($values as $value) {
	$still_running = $value;

	try {
		$result = curl_multi_exec($mh, $still_running);

		var_dump($result);
		var_dump($still_running);
	} catch (Throwable $e) {
		echo get_class($e), ': ', $e->getMessage(), PHP_EOL;
	}
}

curl_multi_close($mh);
?>
--EXPECTF--
int(0)
int(0)
TypeError: curl_multi_exec(): Argument #2 ($still_running) must be of type int, string given

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
TypeError: curl_multi_exec(): Argument #2 ($still_running) must be of type int, array given
TypeError: curl_multi_exec(): Argument #2 ($still_running) must be of type int, stdClass given
