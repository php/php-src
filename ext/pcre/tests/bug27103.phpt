--TEST--
Bug #27103 (preg_split('//u') incorrectly splits UTF-8 strings into octets)
--SKIPIF--
<?php
if (@preg_match('/./u', '') === false) {
    die('skip no utf8 support in PCRE library');
}
?>
--FILE--
<?php
function iter($ar)
{
    foreach ($ar as $c) {
        echo htmlentities($c, 0, "UTF-8"), ": ", strlen($c), "\n";
    }
}
$teststr = "\xe2\x82\xac hi there";
iter(preg_split('//u', $teststr, -1, PREG_SPLIT_NO_EMPTY));
preg_match_all('/./u', $teststr, $matches);
iter($matches[0]);
?>
--EXPECT--
&euro;: 3
 : 1
h: 1
i: 1
 : 1
t: 1
h: 1
e: 1
r: 1
e: 1
&euro;: 3
 : 1
h: 1
i: 1
 : 1
t: 1
h: 1
e: 1
r: 1
e: 1
