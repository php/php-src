--TEST--
setcookie() emits 2 cookies with same name with display_error off
--DESCRIPTION--
--INI--
display_errors=0
--FILE--
<?php
setcookie('name', 'value');
setcookie('name', 'value');

$expected = array(
	'Set-Cookie: name=value',
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

$bad = 0;

foreach ($cookie_headers as $i => $header) {
	if ($header !== $expected[$i]) {
		$bad++;
		echo "Header mismatch:\n\tExpected: "
			. $expected[$i]
			. "\n\tReceived: "
			. $header
			. "\n";
	}
}

echo ($bad === 0)
	? 'OK'
	: 'A total of ' . $bad . ' errors found.';
--EXPECTHEADERS--

--EXPECT--
OK