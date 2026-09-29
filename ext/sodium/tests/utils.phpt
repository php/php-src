--TEST--
Check for libsodium utils
--EXTENSIONS--
sodium
--FILE--
<?php
$a = 'test';
sodium_memzero($a);
if ($a !== 'test') {
  var_dump($a);
} else {
  echo $a;
}
echo "\n";
$b = 'string';
$c = 'string';
var_dump(!sodium_memcmp($b, $c));
var_dump(!sodium_memcmp($b, 'String'));
$v = "\xFF\xFF\x80\x01\x02\x03\x04\x05\x06\x07";
$v .= "\x08";
sodium_increment($v);
var_dump(bin2hex($v));

$w = "\x01\x02\x03\x04\x05\x06\x07\x08\xFA\xFB";
$w .= "\xFC";
sodium_add($v, $w);
var_dump(bin2hex($v));

if (SODIUM_LIBRARY_MAJOR_VERSION > 7 ||
    (SODIUM_LIBRARY_MAJOR_VERSION == 7 &&
     SODIUM_LIBRARY_MINOR_VERSION >= 6)) {
    $v_1 = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F";
    $v_2 = ""."\x02\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F";
    $v_1 .= '';
    var_dump(sodium_compare($v_1, $v_2));
    sodium_increment($v_1);
    var_dump(sodium_compare($v_1, $v_2));
    sodium_increment($v_1);
    var_dump(sodium_compare($v_1, $v_2));
} else {
    // Dummy test results for libsodium < 1.0.4
    var_dump(-1, 0, 1);
}

$str = 'stdClass';
sodium_memzero($str);
$obj = (object)array('foo' => 'bar');
var_dump($obj);

$str = 'xyz';
$str_padded = sodium_pad($str, 16);
var_dump(bin2hex($str_padded));

$str_unpadded = sodium_unpad($str_padded, 16);
var_dump($str_unpadded == $str);

if (defined('SODIUM_BASE64_VARIANT_ORIGINAL')) {
    for ($i = 0; $i < 100; $i++) {
        $bin = $i == 0 ? '' : random_bytes($i);
        $b64 = base64_encode($bin);
        $b64_ = sodium_bin2base64($bin, SODIUM_BASE64_VARIANT_ORIGINAL);
        if ($b64 !== $b64_) {
            echo "frombin[$b64] != frombin_[$b64_]\n";
        }
        $bin_ = sodium_base642bin($b64, SODIUM_BASE64_VARIANT_ORIGINAL);
        if ($bin !== $bin_) {
            echo "frombase64([$b64]) != frombase64_[$b64]\n";
        }
        $bin_ = sodium_base642bin(" $b64\n", SODIUM_BASE64_VARIANT_ORIGINAL, " \n");
        if ($bin !== $bin_) {
            echo "frombase64([$b64]) != frombase64_([ $b64\\n])\n";
        }
    }
    try {
        var_dump(sodium_base642bin('O1R', SODIUM_BASE64_VARIANT_ORIGINAL_NO_PADDING));
    } catch (Exception $e) {
        var_dump('base64("O1R") case passed');
    }
    try {
        var_dump(sodium_base642bin('O1', SODIUM_BASE64_VARIANT_ORIGINAL_NO_PADDING));
    } catch (Exception $e) {
        var_dump('base64("O1") case passed');
    }
    try {
        var_dump(sodium_base642bin('O', SODIUM_BASE64_VARIANT_ORIGINAL_NO_PADDING));
    } catch (Exception $e) {
        var_dump('base64("O") case passed');
    }
    var_dump(sodium_base642bin('YWJjZA', SODIUM_BASE64_VARIANT_ORIGINAL_NO_PADDING));

    $expectedBinary = hex2bin('aeb6798fef4e5184ff3b5596af753487');
    $base64Original = 'rrZ5j+9OUYT/O1WWr3U0hw==';
    $base64OriginalNoPadding = 'rrZ5j+9OUYT/O1WWr3U0hw';
    $base64UrlSafe = 'rrZ5j-9OUYT_O1WWr3U0hw==';
    $base64UrlSafeNoPadding = 'rrZ5j-9OUYT_O1WWr3U0hw';

    $base64 = sodium_bin2base64($expectedBinary, SODIUM_BASE64_VARIANT_ORIGINAL);
    if ($base64 !== $base64Original) {
        echo "Encode original: actual[$base64] != expected[$base64Original]\n";
    }
    $base64 = sodium_bin2base64($expectedBinary, SODIUM_BASE64_VARIANT_ORIGINAL_NO_PADDING);
    if ($base64 !== $base64OriginalNoPadding) {
        echo "Encode original, no padding: actual[$base64] != expected[$base64OriginalNoPadding]\n";
    }
    $base64 = sodium_bin2base64($expectedBinary, SODIUM_BASE64_VARIANT_URLSAFE);
    if ($base64 !== $base64UrlSafe) {
        echo "Encode URL safe: actual[$base64] != expected[$base64UrlSafe]\n";
    }
    $base64 = sodium_bin2base64($expectedBinary, SODIUM_BASE64_VARIANT_URLSAFE_NO_PADDING);
    if ($base64 !== $base64UrlSafeNoPadding) {
        echo "Encode URL safe, no padding: actual[$base64] != expected[$base64UrlSafeNoPadding]\n";
    }

    $binary = sodium_base642bin($base64Original, SODIUM_BASE64_VARIANT_ORIGINAL);
    if ($binary !== $expectedBinary) {
        echo "Decode original: actual[" . bin2hex($binary) . "] != expected[" . bin2hex($expectedBinary) . "]\n";
    }
    $binary = sodium_base642bin(" $base64Original\n", SODIUM_BASE64_VARIANT_ORIGINAL, " \n");
    if ($binary !== $expectedBinary) {
        echo "Decode original, ignore: actual[" . bin2hex($binary) . "] != expected[" . bin2hex($expectedBinary) . "]\n";
    }
    $binary = sodium_base642bin($base64OriginalNoPadding, SODIUM_BASE64_VARIANT_ORIGINAL_NO_PADDING);
    if ($binary !== $expectedBinary) {
        echo "Decode original, no padding: actual[" . bin2hex($binary) . "] != expected[" . bin2hex($expectedBinary) . "]\n";
    }
    $binary = sodium_base642bin(" $base64OriginalNoPadding\n", SODIUM_BASE64_VARIANT_ORIGINAL_NO_PADDING, " \n");
    if ($binary !== $expectedBinary) {
        echo "Decode original, no padding, ignore: actual[" . bin2hex($binary) . "] != expected[" . bin2hex($expectedBinary) . "]\n";
    }
    $binary = sodium_base642bin($base64UrlSafe, SODIUM_BASE64_VARIANT_URLSAFE);
    if ($binary !== $expectedBinary) {
        echo "Decode URL safe: actual[" . bin2hex($binary) . "] != expected[" . bin2hex($expectedBinary) . "]\n";
    }
    $binary = sodium_base642bin(" $base64UrlSafe\n", SODIUM_BASE64_VARIANT_URLSAFE, " \n");
    if ($binary !== $expectedBinary) {
        echo "Decode URL safe, ignore: actual[" . bin2hex($binary) . "] != expected[" . bin2hex($expectedBinary) . "]\n";
    }
    $binary = sodium_base642bin($base64UrlSafeNoPadding, SODIUM_BASE64_VARIANT_URLSAFE_NO_PADDING);
    if ($binary !== $expectedBinary) {
        echo "Decode URL safe, no padding: actual[" . bin2hex($binary) . "] != expected[" . bin2hex($expectedBinary) . "]\n";
    }
    $binary = sodium_base642bin(" $base64UrlSafeNoPadding\n", SODIUM_BASE64_VARIANT_URLSAFE_NO_PADDING, " \n");
    if ($binary !== $expectedBinary) {
        echo "Decode URL safe, no padding, ignore: actual[" . bin2hex($binary) . "] != expected[" . bin2hex($expectedBinary) . "]\n";
    }
} else {
    var_dump('base64("O1R") case passed');
    var_dump('base64("O1") case passed');
    var_dump('base64("O") case passed');
    var_dump('abcd');
}

function sodium_foo()
{
    throw new SodiumException('test');
}

try {
    sodium_foo();
} catch (SodiumException $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}

?>
--EXPECT--
NULL

bool(true)
bool(false)
string(22) "0000810102030405060708"
string(22) "0102840507090b0d000305"
int(-1)
int(0)
int(1)
object(stdClass)#1 (1) {
  ["foo"]=>
  string(3) "bar"
}
string(32) "78797a80000000000000000000000000"
bool(true)
string(25) "base64("O1R") case passed"
string(24) "base64("O1") case passed"
string(23) "base64("O") case passed"
string(4) "abcd"
SodiumException: test
