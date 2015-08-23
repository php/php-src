--TEST--
setcookie() tests
--DESCRIPTION--
--INI--
date.timezone=UTC
--FILE--
<?php
setcookie('name0');
setcookie('name1', 'value');
setcookie('name2', 'space value');
setcookie('name3', 'value', 0);
setcookie('name4', 'value', $tsp = time() + 5);
setcookie('name5', 'value', $tsn = time() - 6);
setcookie('name6', 'value', $tsc = time());
setcookie('name7', 'value', 0, '/path/');
setcookie('name8', 'value', 0, '', 'domain.tld');
setcookie('name9', 'value', 0, '', '', TRUE);
setcookie('name10', 'value', 0, '', '', FALSE, TRUE);


$expected = array(
	'Set-Cookie: name0=',
	'Set-Cookie: name1=value',
	'Set-Cookie: name2=space+value',
	'Set-Cookie: name3=value',
	'Set-Cookie: name4=value; expires='.date('D, d-M-Y H:i:s', $tsp).' GMT; Max-Age=5',
	'Set-Cookie: name5=value; expires='.date('D, d-M-Y H:i:s', $tsn).' GMT; Max-Age=-6',
	'Set-Cookie: name6=value; expires='.date('D, d-M-Y H:i:s', $tsc).' GMT; Max-Age=0',
	'Set-Cookie: name7=value; path=/path/',
	'Set-Cookie: name8=value; domain=domain.tld',
	'Set-Cookie: name9=value; secure',
	'Set-Cookie: name10=value; httponly'
);

$headers = headers_list();
if (($i = count($expected)) > count($headers))
{
	echo "Less headers are being sent than expected - aborting";
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