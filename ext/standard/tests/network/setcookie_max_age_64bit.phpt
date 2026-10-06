--TEST--
setcookie() Max-Age with an expiry beyond the 32bit range
--CGI--
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit int only");
?>
--INI--
date.timezone=UTC
--FILE--
<?php
$far = 4102444800;  /* 2100-01-01 00:00:00 UTC */
$now = time();

setcookie('c', 'v', $far);

foreach (headers_list() as $header) {
    if (!str_starts_with($header, 'Set-Cookie: c=')) {
        continue;
    }

    preg_match('/Max-Age=(-?\d+)/', $header, $m);
    $max_age = (int) $m[1];
    $expected = $far - $now;

    echo 'expires attribute: ',
        str_contains($header, 'expires=Fri, 01 Jan 2100 00:00:00 GMT') ? 'ok' : "WRONG: $header", "\n";
    echo 'Max-Age positive:  ', $max_age > 0 ? 'ok' : "WRONG: $max_age", "\n";
    echo 'Max-Age correct:   ',
        abs($max_age - $expected) <= 2 ? 'ok' : "WRONG: $max_age, expected about $expected", "\n";
}
?>
--EXPECT--
expires attribute: ok
Max-Age positive:  ok
Max-Age correct:   ok
