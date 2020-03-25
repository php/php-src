--TEST--
htmlentities() / htmlspecialchars() ENT_DISALLOWED charset variation
--FILE--
<?php
$tests = array(
    0x00, //C0
    0x01,
    0x09,
    0x0A,
    0x0B,
    0x0C,
    0x0D,
    0x0E,
    0x1F,
    0x20, //allowed always
    0x7F, //DEL
    0x80, //C1
    0x9F,
    0xA0, //allowed always
);

function test($flag, $charset) {
    global $tests;
    $i = -1;
    foreach ($tests as $test) {
        $test = chr($test);
        $i++;
        $a = htmlentities($test, $flag | ENT_DISALLOWED, $charset);
        $b = htmlspecialchars($test, $flag | ENT_DISALLOWED, $charset);
        if ($a == "" && $b == "") { echo sprintf("%05X", $tests[$i]), ": INVALID SEQUENCE\n"; continue; }
        echo sprintf("%05X", $tests[$i]), ": ", bin2hex($a), " ", bin2hex($b), "\n";
    }
}

echo "*** Testing HTML 4.01/Windows-1251 ***\n";

test(ENT_HTML401, "Windows-1251");

echo "\n*** Testing XHTML 1.0/Windows-1251 ***\n";

test(ENT_XHTML, "Windows-1251");

echo "\n*** Testing HTML 5/Windows-1251 ***\n";

test(ENT_HTML5, "Windows-1251");

echo "\n*** Testing XML 1.0/Windows-1251 ***\n";

test(ENT_XML1, "Windows-1251");

echo "\n*** Testing HTML 4.01/SJIS ***\n";

test(ENT_HTML401, "SJIS");

echo "\n*** Testing XHTML 1.0/SJIS ***\n";

test(ENT_XHTML, "SJIS");

echo "\n*** Testing HTML 5/SJIS ***\n";

test(ENT_HTML5, "SJIS");

echo "\n*** Testing XML 1.0/SJIS ***\n";

test(ENT_XML1, "SJIS");


?>
--EXPECTF--
*** Testing HTML 4.01/Windows-1251 ***
00000: 262378464646443b 262378464646443b
00001: 262378464646443b 262378464646443b
00009: 09 09
0000A: 0a 0a
0000B: 262378464646443b 262378464646443b
0000C: 262378464646443b 262378464646443b
0000D: 0d 0d
0000E: 262378464646443b 262378464646443b
0001F: 262378464646443b 262378464646443b
00020: 20 20
0007F: 262378464646443b 7f
00080: 80 80
0009F: 9f 9f
000A0: 266e6273703b a0

*** Testing XHTML 1.0/Windows-1251 ***
00000: 262378464646443b 262378464646443b
00001: 262378464646443b 262378464646443b
00009: 09 09
0000A: 0a 0a
0000B: 262378464646443b 262378464646443b
0000C: 262378464646443b 262378464646443b
0000D: 0d 0d
0000E: 262378464646443b 262378464646443b
0001F: 262378464646443b 262378464646443b
00020: 20 20
0007F: 7f 7f
00080: 80 80
0009F: 9f 9f
000A0: 266e6273703b a0

*** Testing HTML 5/Windows-1251 ***
00000: 262378464646443b 262378464646443b
00001: 262378464646443b 262378464646443b
00009: 265461623b 09
0000A: 264e65774c696e653b 0a
0000B: 262378464646443b 262378464646443b
0000C: 0c 0c
0000D: 0d 0d
0000E: 262378464646443b 262378464646443b
0001F: 262378464646443b 262378464646443b
00020: 20 20
0007F: 262378464646443b 7f
00080: 26444a63793b 80
0009F: 26647a63793b 9f
000A0: 266e6273703b a0

*** Testing XML 1.0/Windows-1251 ***
00000: 262378464646443b 262378464646443b
00001: 262378464646443b 262378464646443b
00009: 09 09
0000A: 0a 0a
0000B: 262378464646443b 262378464646443b
0000C: 262378464646443b 262378464646443b
0000D: 0d 0d
0000E: 262378464646443b 262378464646443b
0001F: 262378464646443b 262378464646443b
00020: 20 20
0007F: 7f 7f
00080: 80 80
0009F: 9f 9f
000A0: a0 a0

*** Testing HTML 4.01/SJIS ***

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00000: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00001: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00009: 09 09

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000A: 0a 0a

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000B: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000C: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000D: 0d 0d

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000E: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0001F: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00020: 20 20

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0007F: 7f 7f

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00080: INVALID SEQUENCE

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0009F: INVALID SEQUENCE

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
000A0: INVALID SEQUENCE

*** Testing XHTML 1.0/SJIS ***

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00000: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00001: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00009: 09 09

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000A: 0a 0a

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000B: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000C: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000D: 0d 0d

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000E: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0001F: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00020: 20 20

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0007F: 7f 7f

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00080: INVALID SEQUENCE

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0009F: INVALID SEQUENCE

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
000A0: INVALID SEQUENCE

*** Testing HTML 5/SJIS ***

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00000: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00001: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00009: 09 09

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000A: 0a 0a

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000B: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000C: 0c 0c

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000D: 0d 0d

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000E: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0001F: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00020: 20 20

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0007F: 7f 7f

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00080: INVALID SEQUENCE

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0009F: INVALID SEQUENCE

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
000A0: INVALID SEQUENCE

*** Testing XML 1.0/SJIS ***

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00000: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00001: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00009: 09 09

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000A: 0a 0a

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000B: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000C: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000D: 0d 0d

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0000E: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0001F: 262378464646443b 262378464646443b

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00020: 20 20

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0007F: 7f 7f

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
00080: INVALID SEQUENCE

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
0009F: INVALID SEQUENCE

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
000A0: INVALID SEQUENCE
