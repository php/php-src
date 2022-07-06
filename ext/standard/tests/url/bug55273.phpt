--TEST--
Bug #55273 (base64_decode() with strict rejects whitespace after pad)
--FILE--
<?php
function test($s) {
    $v = chunk_split(base64_encode($s));
    $r = base64_decode($v, True);
    var_dump($v, $r);
}

test('PHP');
test('PH');
test('P');

?>
--EXPECT--
string(6) "UEhQ
"
string(3) "PHP"
string(6) "UEg=
"
string(2) "PH"
string(6) "UA==
"
string(1) "P"
