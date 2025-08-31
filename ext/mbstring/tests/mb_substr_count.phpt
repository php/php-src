--TEST--
mb_substr_count()
--EXTENSIONS--
mbstring
--INI--
output_handler=
--FILE--
<?php
    mb_internal_encoding("EUC-JP");

    print "== Empty needle should raise an error ==\n";
    try {
        var_dump(mb_substr_count("", ""));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    try {
        var_dump(mb_substr_count("��", ""));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    try {
        // Although the needle below contains 3 bytes, it decodes to zero Unicode codepoints
        // So the needle is actually 'empty', although it doesn't appear so
        var_dump(mb_substr_count("abcdef", "\x1B(B", "ISO-2022-JP"));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    print "== Return value for empty haystack should always be zero ==\n";
    var_dump(mb_substr_count("", "\xA4\xA2"));
    var_dump(mb_substr_count("", chr(0)));

    print "== Try searching using various encodings ==\n";
    $a = str_repeat("abcacba", 100);
    var_dump(mb_substr_count($a, "bca"));

    $a = str_repeat("\xA4\xA2\xA4\xA4\xA4\xA6\xA4\xA2\xA4\xA6\xA4\xA4\xA4\xA2", 100);
    $b = "\xA4\xA4\xA4\xA6\xA4\xA2";
    var_dump(mb_substr_count($a, $b));

    $to_enc = "UTF-8";
    var_dump(mb_substr_count(mb_convert_encoding($a, $to_enc),
                              mb_convert_encoding($b, $to_enc), $to_enc));

    $to_enc = "Shift_JIS";
    var_dump(mb_substr_count(mb_convert_encoding($a, $to_enc),
                              mb_convert_encoding($b, $to_enc), $to_enc));

    $a = str_repeat("abcacbabca", 100);
    var_dump(mb_substr_count($a, "bca"));

    print "== Regression tests ==\n";

    // The old implementation had a bug; it could only recognize a maximum of one
    // match for each byte that it fed into the decoder, even if feeding in that
    // byte caused two codepoints to be emitted (because the decoder was holding
    // cached data), and both of those codepoints matched a 1-codepoint needle
    // (For this example, two error markers are emitted for the final byte 0xFF)
    echo mb_substr_count("\xef\xff", "\xf8", "UTF-8"), "\n";

    // Another thing about the old implementation: if a final codepoint was emitted
    // by a decoder flush function, and that codepoint finished a match with the
    // needle, that match would be disregarded and not counted in the returned value
    // (In practice, the only thing emitted from decoder flush functions is an error
    // marker, if the string ended in an illegal state)
    echo mb_substr_count("+", "+", "UTF7-IMAP"), "\n";

?>
--EXPECT--
== Empty needle should raise an error ==
mb_substr_count(): Argument #2 ($needle) must not be empty
mb_substr_count(): Argument #2 ($needle) must not be empty
mb_substr_count(): Argument #2 ($needle) must not be empty
== Return value for empty haystack should always be zero ==
int(0)
int(0)
== Try searching using various encodings ==
int(100)
int(100)
int(100)
int(100)
int(200)
== Regression tests ==
2
1
