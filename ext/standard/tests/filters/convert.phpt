--TEST--
convert stream filter tests
--FILE--
<?php

$text = "Hello World!";
$binary = "ABC\xFF";

function test_roundtrip($data, $encode, $decode, $opts = null, $is_binary = false) {
    echo "Filter: $encode\n";

    $fp = tmpfile();
    fwrite($fp, $data);
    rewind($fp);

    if ($opts === null) {
        stream_filter_prepend($fp, $encode, STREAM_FILTER_READ);
    } else {
        stream_filter_prepend($fp, $encode, STREAM_FILTER_READ, $opts);
    }
    $encoded = stream_get_contents($fp);
    fclose($fp);

    if ($is_binary) {
        echo "Original (hex): " . bin2hex($data) . "\n";
        echo "Encoded: $encoded\n";
    } else {
        echo "Original: $data\n";
        echo "Encoded: $encoded\n";
    }

    $fp2 = tmpfile();
    fwrite($fp2, $encoded);
    rewind($fp2);

    if ($opts === null) {
        stream_filter_prepend($fp2, $decode, STREAM_FILTER_READ);
    } else {
        stream_filter_prepend($fp2, $decode, STREAM_FILTER_READ, $opts);
    }
    $decoded = stream_get_contents($fp2);
    fclose($fp2);

    if ($is_binary) {
        echo "Decoded (hex): " . bin2hex($decoded) . "\n";
    } else {
        echo "Decoded: $decoded\n";
    }
    var_dump($data === $decoded);
    echo "\n";
}

test_roundtrip($text, "convert.base64-encode", "convert.base64-decode");
test_roundtrip($binary, "convert.base64-encode", "convert.base64-decode", null, true);
test_roundtrip("", "convert.base64-encode", "convert.base64-decode");
test_roundtrip("A", "convert.base64-encode", "convert.base64-decode");

test_roundtrip($text, "convert.quoted-printable-encode", "convert.quoted-printable-decode");
test_roundtrip("Line1\r\nLine2", "convert.quoted-printable-encode", "convert.quoted-printable-decode");

$opts = array('line-length' => 20, 'line-break-chars' => "\n");
test_roundtrip("Long text that will be wrapped", "convert.base64-encode", "convert.base64-decode", $opts);

$opts2 = array('binary' => true);
test_roundtrip("Text\t\r\n", "convert.quoted-printable-encode", "convert.quoted-printable-decode", $opts2);

$fp = tmpfile();
fwrite($fp, "Test");
rewind($fp);
stream_filter_prepend($fp, 'convert.base64-encode', STREAM_FILTER_READ);
$result = stream_get_contents($fp);
fclose($fp);
var_dump($result === base64_encode("Test"));

?>
--EXPECTF--
Filter: convert.base64-encode
Original: Hello World!
Encoded: SGVsbG8gV29ybGQh
Decoded: Hello World!
bool(true)

Filter: convert.base64-encode
Original (hex): 414243ff
Encoded: QUJD/w==
Decoded (hex): 414243ff
bool(true)

Filter: convert.base64-encode
Original: 
Encoded: 
Decoded: 
bool(true)

Filter: convert.base64-encode
Original: A
Encoded: QQ==
Decoded: A
bool(true)

Filter: convert.quoted-printable-encode
Original: Hello World!
Encoded: Hello World!
Decoded: Hello World!
bool(true)

Filter: convert.quoted-printable-encode
Original: Line1
Line2
Encoded: Line1=0D=0ALine2
Decoded: Line1
Line2
bool(true)

Filter: convert.base64-encode
Original: Long text that will be wrapped
Encoded: TG9uZyB0ZXh0IHRoYXQg
d2lsbCBiZSB3cmFwcGVk
Decoded: Long text that will be wrapped
bool(true)

Filter: convert.quoted-printable-encode
Original: Text	

Encoded: Text=09=0D=0A
Decoded: Text	

bool(true)

bool(true)
