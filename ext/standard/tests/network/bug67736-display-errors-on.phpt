--TEST--
setcookie() emits 1 cookie then adds a warning for 2nd with same name
--DESCRIPTION--
--INI--
--FILE--
<?php

setcookie('name', 'value');
setcookie('name', 'value');

$expected = array(
	'Set-Cookie: name=value',
);

$headers = headers_list();

// Filter to get only the Set-Cookie headers
$cookie_headers = [];
foreach ($headers as $header) {
	if (strpos($header, 'Set-Cookie:') === 0) $cookie_headers[] = $header;
}

if (count($cookie_headers) !== count($expected)) {
	echo "Less headers are being sent than expected - aborting";
	return;
}
--EXPECTHEADERS--

--EXPECTF--

Warning: setcookie(): should not be used twice with the same name in %s

Warning: Cannot modify header information - headers already sent by %s
