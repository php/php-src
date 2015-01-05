--TEST--
Bug #47174 (base64_decode() interprets pad char in mid string as terminator)
--FILE--
<?php
if (base64_decode(b"dGVzdA==") == base64_decode(b"dGVzdA==CRAP")) {
    echo "Same octect data - Signature Valid\n";
} else {
    echo "Invalid Signature\n";
}

$in = base64_encode(b"foo") . b'==' . base64_encode(b"bar");
var_dump($in, base64_decode($in));

?>
--EXPECT--
Invalid Signature
string(10) "Zm9v==YmFy"
string(6) "foobar"
