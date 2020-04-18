--TEST--
mb_substr_count()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
--FILE--
<?php
    mb_internal_encoding("EUC-JP");
    try {
        var_dump(mb_substr_count("", ""));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    try {
        var_dump(mb_substr_count("鐃緒申", ""));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    var_dump(mb_substr_count("", "あ"));
    var_dump(mb_substr_count("", "あ"));
    var_dump(mb_substr_count("", chr(0)));

    $a = str_repeat("abcacba", 100);
    var_dump(@mb_substr_count($a, "bca"));

    $a = str_repeat("あいうあういあ", 100);
    $b = "いうあ";
    var_dump(@mb_substr_count($a, $b));

    $to_enc = "UTF-8";
    var_dump(@mb_substr_count(mb_convert_encoding($a, $to_enc),
                              mb_convert_encoding($b, $to_enc), $to_enc));

    $to_enc = "Shift_JIS";
    var_dump(@mb_substr_count(mb_convert_encoding($a, $to_enc),
                              mb_convert_encoding($b, $to_enc), $to_enc));

    $a = str_repeat("abcacbabca", 100);
    var_dump(@mb_substr_count($a, "bca"));
?>
--EXPECT--
mb_substr_count(): Argument #2 ($needle) must not be empty
mb_substr_count(): Argument #2 ($needle) must not be empty
int(0)
int(0)
int(0)
int(100)
int(100)
int(100)
int(100)
int(200)
