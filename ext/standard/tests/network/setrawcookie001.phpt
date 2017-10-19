--TEST--
setrawcookie() tests
--DESCRIPTION--
--INI--
date.timezone=UTC
--FILE--
<?php
setrawcookie('name');
setrawcookie('name', '');
setrawcookie('name', 'value');
setrawcookie('name', 'space+value');
setrawcookie('name', 'value', 0);
setrawcookie('name', 'value', $tsp = time() + 5);
setrawcookie('name', 'value', $tsn = time() - 6);
setrawcookie('name', 'value', $tsc = time());
setrawcookie('name', 'value', 0, '/path/');
setrawcookie('name', 'value', 0, '', 'domain.tld');
setrawcookie('name', 'value', 0, '', '', TRUE);
setrawcookie('name', 'value', 0, '', '', FALSE, TRUE);


$expected = array(
	'Set-Cookie: name=deleted; expires='.date('D, d-M-Y H:i:s', 1).' GMT; Max-Age=0',
	'Set-Cookie: name=deleted; expires='.date('D, d-M-Y H:i:s', 1).' GMT; Max-Age=0',
	'Set-Cookie: name=value',
	'Set-Cookie: name=space+value',
	'Set-Cookie: name=value',
	'Set-Cookie: name=value; expires='.date('D, d-M-Y H:i:s', $tsp).' GMT; Max-Age=5',
	'Set-Cookie: name=value; expires='.date('D, d-M-Y H:i:s', $tsn).' GMT; Max-Age=0',
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
?>
--EXPECTHEADERS--

--EXPECT--
OK
--CREDITS--
Bill Condo bill@billcondo.com
Matthew Radcliffe mradcliffe@kosada.com
Peter Barney peter@peterbarney.com
Michael Cordingley michael.cordingley@gmail.com
Taylor Howard samueltaylorhoward@gmail.com
# TestFest Columbus PHP UG 2017-10-11