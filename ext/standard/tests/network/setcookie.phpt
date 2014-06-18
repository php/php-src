--TEST--
setcookie() tests
--DESCRIPTION--
--INI--
date.timezone=UTC
--FILE--
<?php
setcookie('name');
setcookie('name', 'value');
setcookie('name', 'space value');
setcookie('name', 'value', 0);
setcookie('name', 'value', $tsp = time() + 5);
setcookie('name', 'value', $tsn = time() - 6);
setcookie('name', 'value', $tsc = time());
setcookie('name', 'value', 0, '/path/');
setcookie('name', 'value', 0, '', 'domain.tld');
setcookie('name', 'value', 0, '', '', TRUE);
setcookie('name', 'value', 0, '', '', FALSE, TRUE);


$expected = array(
	'Set-Cookie: name=',
	'Set-Cookie: name=value',
	'Set-Cookie: name=space+value',
	'Set-Cookie: name=value',
	'Set-Cookie: name=value; expires='.date('D, d-M-Y H:i:s', $tsp).' GMT; Max-Age=5',
	'Set-Cookie: name=value; expires='.date('D, d-M-Y H:i:s', $tsn).' GMT; Max-Age=-6',
	'Set-Cookie: name=value; expires='.date('D, d-M-Y H:i:s', $tsc).' GMT; Max-Age=0',
	'Set-Cookie: name=value; path=/path/',
	'Set-Cookie: name=value; domain=domain.tld',
	'Set-Cookie: name=value; secure',
	'Set-Cookie: name=value; HttpOnly'
);

$headers = headers_list();
if (($i = count($expected)) > count($headers))
{
	echo "Fewer headers are being sent than expected - aborting";
	return;
}

do
{
	if (strncmp(current($headers), 'Set-Cookie:', 11) !== 0)
	{
		continue;
	}

	if (current($headers) === current($expected))
	{
		$i--;
	}
	else
	{
		echo "Header mismatch:\n\tExpected: "
			.current($expected)
			."\n\tReceived: ".current($headers)."\n";
	}

	next($expected);
}
while (next($headers) !== FALSE);

echo ($i === 0)
	? 'OK'
	: 'A total of '.$i.' errors found.';
--EXPECTHEADERS--

--EXPECT--
OK
