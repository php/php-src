--TEST--
setcookie() tests
--INI--
date.timezone=UTC
--FILE--
<?php
setcookie('name');
setcookie('name', '');
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

setcookie('name', 'value', ['expires' => $tsp]);
setcookie('name', 'value', ['expires' => $tsn, 'path' => '/path/', 'domain' => 'domain.tld', 'secure' => true, 'httponly' => true, 'samesite' => 'Strict']);

$expected = array(
    'Set-Cookie: name=deleted; expires='.date('D, d M Y H:i:s', 1).' GMT; Max-Age=0',
    'Set-Cookie: name=deleted; expires='.date('D, d M Y H:i:s', 1).' GMT; Max-Age=0',
    'Set-Cookie: name=value',
    'Set-Cookie: name=space%20value',
    'Set-Cookie: name=value',
    'Set-Cookie: name=value; expires='.date('D, d M Y H:i:s', $tsp).' GMT; Max-Age=5',
    'Set-Cookie: name=value; expires='.date('D, d M Y H:i:s', $tsn).' GMT; Max-Age=0',
    'Set-Cookie: name=value; expires='.date('D, d M Y H:i:s', $tsc).' GMT; Max-Age=0',
    'Set-Cookie: name=value; path=/path/',
    'Set-Cookie: name=value; domain=domain.tld',
    'Set-Cookie: name=value; secure',
    'Set-Cookie: name=value; HttpOnly',
    'Set-Cookie: name=value; expires='.date('D, d M Y H:i:s', $tsp).' GMT; Max-Age=5',
    'Set-Cookie: name=value; expires='.date('D, d M Y H:i:s', $tsn).' GMT; Max-Age=0; path=/path/; domain=domain.tld; secure; HttpOnly; SameSite=Strict'
);

$headers = headers_list();
if (($i = count($expected)) > count($headers))
{
    echo "Fewer headers are being sent than expected - aborting";
    return;
}

do {
    $header = current($headers);
    if (strncmp($header, 'Set-Cookie:', 11) !== 0) {
        continue;
    }

    // If the second rolls over between the time() call and the internal time determination by
    // setcookie(), we might get Max-Age=4 instead of Max-Age=5.
    $header = str_replace('Max-Age=4', 'Max-Age=5', $header);
    if ($header === current($expected)) {
        $i--;
    } else {
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
