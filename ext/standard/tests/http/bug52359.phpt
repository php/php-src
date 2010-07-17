--TEST--
Bug #52359 (http_build_query seem to encode array incorrectly)
--FILE--
<?php
$a = array(
    'foo/bar' => array(
        '1',
        '2'
    ),
    'bar' => 'foo'
);

$b = http_build_query($a, NULL, '&');
var_dump($b);

$c = urldecode($b);
var_dump($c);
?>
--EXPECTF--
string(43) "foo%2Fbar%5B%5D=1&foo%2Fbar%5B%5D=2&bar=foo"
string(31) "foo/bar[]=1&foo/bar[]=2&bar=foo"