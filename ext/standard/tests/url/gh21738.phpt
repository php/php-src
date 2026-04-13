--TEST--
GH-21738 (Avoid UB when urldecode() inspects non-ASCII bytes)
--FILE--
<?php

$tests = [
    "%\x80A",
    "%A\x80",
    "%\xFFA",
    "%F\xFF",
];

foreach ($tests as $test) {
    var_dump(bin2hex(urldecode($test)));
    var_dump(bin2hex(rawurldecode($test)));
}

?>
--EXPECT--
string(6) "258041"
string(6) "258041"
string(6) "254180"
string(6) "254180"
string(6) "25ff41"
string(6) "25ff41"
string(6) "2546ff"
string(6) "2546ff"
