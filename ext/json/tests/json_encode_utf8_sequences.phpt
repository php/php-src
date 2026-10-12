--TEST--
json_encode() with valid and invalid UTF-8 sequences around the two-, three- and four-byte boundaries
--FILE--
<?php
$cases = [
    'C2 80, smallest two-byte' => "\xC2\x80",
    'DF BF, largest two-byte' => "\xDF\xBF",
    'C0 AF, overlong' => "\xC0\xAF",
    'C1 BF, overlong' => "\xC1\xBF",
    '80, lone continuation byte' => "\x80",
    'C2 at the end' => "a\xC2",
    'C2 then ASCII' => "\xC2a",
    'C2 then lead byte' => "\xC2\xC3\xA9",
    'E0 A0 80, smallest three-byte' => "\xE0\xA0\x80",
    'E0 9F BF, overlong' => "\xE0\x9F\xBF",
    'ED 9F BF, before surrogates' => "\xED\x9F\xBF",
    'ED A0 80, surrogate' => "\xED\xA0\x80",
    'ED BF BF, surrogate' => "\xED\xBF\xBF",
    'ED A0 at the end' => "a\xED\xA0",
    'EE 80 80, after surrogates' => "\xEE\x80\x80",
    'EF BF BF, largest three-byte' => "\xEF\xBF\xBF",
    'E2 82 at the end' => "a\xE2\x82",
    'E2 82 then ASCII' => "\xE2\x82A",
    'E2 then ASCII' => "\xE2ab",
    'E2 80 A8, U+2028' => "\xE2\x80\xA8",
    'F0 90 80 80, smallest four-byte' => "\xF0\x90\x80\x80",
    'F4 8F BF BF, largest four-byte' => "\xF4\x8F\xBF\xBF",
    'F0 8F BF BF, overlong' => "\xF0\x8F\xBF\xBF",
    'F4 90 80 80, above U+10FFFF' => "\xF4\x90\x80\x80",
    'F5 80 80 80' => "\xF5\x80\x80\x80",
    'F0 9F 98 at the end' => "a\xF0\x9F\x98",
    'F0 9F 98 then ASCII' => "\xF0\x9F\x98a",
    'F0 9F 98 then lead byte' => "\xF0\x9F\x98\xF0\x9F\x98\x80",
    'four-byte between ASCII' => "x\xF0\x9F\x98\x80y",
    'two-byte after ASCII and at the end' => "abc\xD0\xB6",
    'quote after two-byte' => "\xD0\xB6\"\xD0\xB6/",
    'three-byte between ASCII' => "x\xE6\xBC\xA2y",
];
foreach ($cases as $name => $str) {
    echo "$name:\n";
    var_dump(json_encode($str));
    var_dump(json_encode($str, JSON_INVALID_UTF8_IGNORE));
    var_dump(json_encode($str, JSON_INVALID_UTF8_SUBSTITUTE));
    var_dump(bin2hex(json_encode($str, JSON_UNESCAPED_UNICODE | JSON_INVALID_UTF8_SUBSTITUTE)));
}
echo "U+2028 with JSON_UNESCAPED_LINE_TERMINATORS:\n";
var_dump(bin2hex(json_encode("a\xE2\x80\xA8b", JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_LINE_TERMINATORS)));
echo "JSON_PARTIAL_OUTPUT_ON_ERROR:\n";
var_dump(json_encode(["ok\xD0\xB6", "bad\xC0\xAF", "\xE2\x82"], JSON_PARTIAL_OUTPUT_ON_ERROR));
var_dump(json_last_error_msg());
?>
--EXPECT--
C2 80, smallest two-byte:
string(8) ""\u0080""
string(8) ""\u0080""
string(8) ""\u0080""
string(8) "22c28022"
DF BF, largest two-byte:
string(8) ""\u07ff""
string(8) ""\u07ff""
string(8) ""\u07ff""
string(8) "22dfbf22"
C0 AF, overlong:
bool(false)
string(2) """"
string(14) ""\ufffd\ufffd""
string(16) "22efbfbdefbfbd22"
C1 BF, overlong:
bool(false)
string(2) """"
string(14) ""\ufffd\ufffd""
string(16) "22efbfbdefbfbd22"
80, lone continuation byte:
bool(false)
string(2) """"
string(8) ""\ufffd""
string(10) "22efbfbd22"
C2 at the end:
bool(false)
string(3) ""a""
string(9) ""a\ufffd""
string(12) "2261efbfbd22"
C2 then ASCII:
bool(false)
string(3) ""a""
string(9) ""\ufffda""
string(12) "22efbfbd6122"
C2 then lead byte:
bool(false)
string(8) ""\u00e9""
string(14) ""\ufffd\u00e9""
string(14) "22efbfbdc3a922"
E0 A0 80, smallest three-byte:
string(8) ""\u0800""
string(8) ""\u0800""
string(8) ""\u0800""
string(10) "22e0a08022"
E0 9F BF, overlong:
bool(false)
string(2) """"
string(8) ""\ufffd""
string(10) "22efbfbd22"
ED 9F BF, before surrogates:
string(8) ""\ud7ff""
string(8) ""\ud7ff""
string(8) ""\ud7ff""
string(10) "22ed9fbf22"
ED A0 80, surrogate:
bool(false)
string(2) """"
string(8) ""\ufffd""
string(10) "22efbfbd22"
ED BF BF, surrogate:
bool(false)
string(2) """"
string(8) ""\ufffd""
string(10) "22efbfbd22"
ED A0 at the end:
bool(false)
string(3) ""a""
string(9) ""a\ufffd""
string(12) "2261efbfbd22"
EE 80 80, after surrogates:
string(8) ""\ue000""
string(8) ""\ue000""
string(8) ""\ue000""
string(10) "22ee808022"
EF BF BF, largest three-byte:
string(8) ""\uffff""
string(8) ""\uffff""
string(8) ""\uffff""
string(10) "22efbfbf22"
E2 82 at the end:
bool(false)
string(3) ""a""
string(9) ""a\ufffd""
string(12) "2261efbfbd22"
E2 82 then ASCII:
bool(false)
string(3) ""A""
string(9) ""\ufffdA""
string(12) "22efbfbd4122"
E2 then ASCII:
bool(false)
string(4) ""ab""
string(10) ""\ufffdab""
string(14) "22efbfbd616222"
E2 80 A8, U+2028:
string(8) ""\u2028""
string(8) ""\u2028""
string(8) ""\u2028""
string(16) "225c753230323822"
F0 90 80 80, smallest four-byte:
string(14) ""\ud800\udc00""
string(14) ""\ud800\udc00""
string(14) ""\ud800\udc00""
string(12) "22f090808022"
F4 8F BF BF, largest four-byte:
string(14) ""\udbff\udfff""
string(14) ""\udbff\udfff""
string(14) ""\udbff\udfff""
string(12) "22f48fbfbf22"
F0 8F BF BF, overlong:
bool(false)
string(2) """"
string(8) ""\ufffd""
string(10) "22efbfbd22"
F4 90 80 80, above U+10FFFF:
bool(false)
string(2) """"
string(8) ""\ufffd""
string(10) "22efbfbd22"
F5 80 80 80:
bool(false)
string(2) """"
string(26) ""\ufffd\ufffd\ufffd\ufffd""
string(28) "22efbfbdefbfbdefbfbdefbfbd22"
F0 9F 98 at the end:
bool(false)
string(3) ""a""
string(9) ""a\ufffd""
string(12) "2261efbfbd22"
F0 9F 98 then ASCII:
bool(false)
string(3) ""a""
string(9) ""\ufffda""
string(12) "22efbfbd6122"
F0 9F 98 then lead byte:
bool(false)
string(14) ""\ud83d\ude00""
string(20) ""\ufffd\ud83d\ude00""
string(18) "22efbfbdf09f988022"
four-byte between ASCII:
string(16) ""x\ud83d\ude00y""
string(16) ""x\ud83d\ude00y""
string(16) ""x\ud83d\ude00y""
string(16) "2278f09f98807922"
two-byte after ASCII and at the end:
string(11) ""abc\u0436""
string(11) ""abc\u0436""
string(11) ""abc\u0436""
string(14) "22616263d0b622"
quote after two-byte:
string(18) ""\u0436\"\u0436\/""
string(18) ""\u0436\"\u0436\/""
string(18) ""\u0436\"\u0436\/""
string(20) "22d0b65c22d0b65c2f22"
three-byte between ASCII:
string(10) ""x\u6f22y""
string(10) ""x\u6f22y""
string(10) ""x\u6f22y""
string(14) "2278e6bca27922"
U+2028 with JSON_UNESCAPED_LINE_TERMINATORS:
string(14) "2261e280a86222"
JSON_PARTIAL_OUTPUT_ON_ERROR:
string(22) "["ok\u0436",null,null]"
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
