--TEST--
mb_strcut()
--EXTENSIONS--
mbstring
--INI--
output_handler=
--FILE--
<?php
function MBStringChars($string, $encoding) {
  $chars = mb_str_split($string, 1, $encoding);
  return '[' . implode(' ', array_map(function($char) {
    return join(unpack('H*', $char));
  }, $chars)) . ']';
}

ini_set('include_path', __DIR__);
include_once('common.inc');

$euc_jp = pack('H*', '30313233a4b3a4cecab8bbfacef3a4cfc6fccbdcb8eca4c7a4b9a1a34555432d4a50a4f2bbc8a4c3a4c6a4a4a4dea4b9a1a3c6fccbdcb8eca4cfccccc5ddbdada4a4a1a3');
$utf8 = pack('H*', 'e288ae2045e28b856461203d2051'); // has 2 multi-byte characters: [e288ae 20 45 e28b85 64 61 20 3d 20 51]
$utf16le = pack('H*', '1a043804400438043b043b04380446043004200069007300200043007900720069006c006c0069006300');
$utf32be = mb_convert_encoding($utf8, 'UTF-32BE', 'UTF-8');
$iso2022jp = mb_convert_encoding("漢字 abc カナ", 'ISO-2022-JP', 'UTF-8'); // [<escape sequence>1b2442 3441 3b7a <escape sequence>1b2842 20 61 62 63 20 <escape sequence>1b2442 252b 254a <escape sequence>1b2842]
$jis = mb_convert_encoding("漢字 abc カナ", 'JIS', 'UTF-8');
// For testing ISO-2022-JP-2004, add a Kanji character which is in JISX 0213
$iso2022jp2004 = mb_convert_encoding("漢字 abc カナ凜", 'ISO-2022-JP-2004', 'UTF-8'); // [1b242851 3441 3b7a 1b2842 20 61 62 63 20 1b242851 252b 254a 7425 1b2842]
$iso2022jpms = mb_convert_encoding("漢字 abc カナ", 'ISO-2022-JP-MS', 'UTF-8'); // [1b2442 3441 3b7a 1b2842 20 61 62 63 20 1b2442 252b 254a 1b2842]
$iso2022jp_kddi = mb_convert_encoding("漢字 abc カナ", 'ISO-2022-JP-KDDI', 'UTF-8');

print "== EUC-JP ==\n";
print MBStringChars(mb_strcut($euc_jp,  6,   5, 'EUC-JP'), 'EUC-JP') . "\n";
print MBStringChars(mb_strcut($euc_jp,  5,   5, 'EUC-JP'), 'EUC-JP') . "\n";
print MBStringChars(mb_strcut($euc_jp,  0, 100, 'EUC-JP'), 'EUC-JP') . "\n";

$str = mb_strcut($euc_jp, 100, 10, 'EUC-JP');
($str === "") ? print "OK\n" : print "No good\n";

$str = mb_strcut($euc_jp, -100, 10, 'EUC-JP');
($str !== "") ?	print "OK\n" : print "No good\n";

print "== UTF-8 ==\n";
print MBStringChars(mb_strcut($utf8, 0, 0, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 0, 1, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 0, 2, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 0, 3, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 0, 4, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 0, 5, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 1, 2, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 1, 3, 'UTF-8'), 'UTF-8') . "\n";
print MBStringChars(mb_strcut($utf8, 1, 4, 'UTF-8'), 'UTF-8') . "\n";

print MBStringChars(mb_strcut('AåBäCöDü', 2, 100, 'UTF-8'), 'UTF-8') . "\n";

print "== UTF-16 ==\n";
print "Single byte: [" . bin2hex(mb_strcut("\xFF", 0, 100, 'UTF-16')) . "]\n";
print "With from=1: [" . bin2hex(mb_strcut("\xff\x01", 1, 100, "UTF-16")) . "]\n";
print "Bad surrogate: [" . bin2hex(mb_strcut("\xD9\xFF", 0, 100, "UTF-16")) . "]\n";
print "Bad surrogate followed by other bytes: [" . bin2hex(mb_strcut("\xd9\x00\x12C", 0, 100, "UTF-16")) . "]\n";
print "BE byte order mark: [" . bin2hex(mb_strcut("\xFE\xFF", 0, 100, "UTF-16")) . "]\n";
print "LE byte order mark: [" . bin2hex(mb_strcut("\xFF\xFE", 0, 100, "UTF-16")) . "]\n";
print "Length=0: [" . bin2hex(mb_strcut("\x00\x01\x00\x00", 1, -512, "UTF-16")) . "]\n";

print "== UTF-16LE ==\n";
print MBStringChars(mb_strcut($utf16le, 0, 0, 'UTF-16LE'), 'UTF-16LE') . "\n";
print MBStringChars(mb_strcut($utf16le, 0, 1, 'UTF-16LE'), 'UTF-16LE') . "\n";
print MBStringChars(mb_strcut($utf16le, 0, 2, 'UTF-16LE'), 'UTF-16LE') . "\n";
print MBStringChars(mb_strcut($utf16le, 0, 3, 'UTF-16LE'), 'UTF-16LE') . "\n";
print MBStringChars(mb_strcut($utf16le, 1, 2, 'UTF-16LE'), 'UTF-16LE') . "\n";
print MBStringChars(mb_strcut($utf16le, 1, 3, 'UTF-16LE'), 'UTF-16LE') . "\n";
print MBStringChars(mb_strcut($utf16le, 1, 4, 'UTF-16LE'), 'UTF-16LE') . "\n";

print "Single byte: [" . bin2hex(mb_strcut("\xFF", 0, 100, 'UTF-16LE')) . "]\n";

print "== UTF-32BE ==\n";
print MBStringChars(mb_strcut($utf32be, 0, 3, 'UTF-32BE'), 'UTF-32BE') . "\n";
print MBStringChars(mb_strcut($utf32be, 0, 4, 'UTF-32BE'), 'UTF-32BE') . "\n";
print MBStringChars(mb_strcut($utf32be, 0, 5, 'UTF-32BE'), 'UTF-32BE') . "\n";
print MBStringChars(mb_strcut($utf32be, 1, 8, 'UTF-32BE'), 'UTF-32BE') . "\n";
print MBStringChars(mb_strcut($utf32be, 3, 9, 'UTF-32BE'), 'UTF-32BE') . "\n";

print "== ISO-2022-JP ==\n";
print MBStringChars(mb_strcut($iso2022jp, 0, 3, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";
print MBStringChars(mb_strcut($iso2022jp, 0, 4, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";
print MBStringChars(mb_strcut($iso2022jp, 0, 5, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";
print MBStringChars(mb_strcut($iso2022jp, 0, 6, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";
print MBStringChars(mb_strcut($iso2022jp, 0, 7, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";
print MBStringChars(mb_strcut($iso2022jp, 0, 8, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";

print MBStringChars(mb_strcut($iso2022jp, 1, 3, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";
print MBStringChars(mb_strcut($iso2022jp, 1, 6, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";
print MBStringChars(mb_strcut($iso2022jp, 1, 8, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";

print MBStringChars(mb_strcut($iso2022jp, 2, 5, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";
print MBStringChars(mb_strcut($iso2022jp, 5, 9, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";
print MBStringChars(mb_strcut($iso2022jp, 5, 11, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";
print MBStringChars(mb_strcut($iso2022jp, 6, 13, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";
print MBStringChars(mb_strcut($iso2022jp, 7, 13, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";

print MBStringChars(mb_strcut($iso2022jp, 1, 100, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";
print MBStringChars(mb_strcut($iso2022jp, 50, 100, 'ISO-2022-JP'), 'ISO-2022-JP') . "\n";

print "Error followed by ASCII char: [" . bin2hex(mb_strcut("\xdaK", 0, 100, "ISO-2022-JP")) . "]\n";

print "== ISO-2022-JP-2004 ==\n";
print MBStringChars(mb_strcut($iso2022jp2004, 0, 3, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 0, 4, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 0, 5, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 0, 6, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 0, 7, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 0, 8, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 0, 9, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";

print MBStringChars(mb_strcut($iso2022jp2004, 1, 3, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 1, 6, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 1, 8, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 1, 9, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";

print MBStringChars(mb_strcut($iso2022jp2004, 2, 5, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 5, 9, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 5, 11, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 6, 13, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 7, 13, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";

print MBStringChars(mb_strcut($iso2022jp2004, 1, 100, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";
print MBStringChars(mb_strcut($iso2022jp2004, 50, 100, 'ISO-2022-JP-2004'), 'ISO-2022-JP-2004') . "\n";

print "== ISO-2022-JP-MS ==\n";
print MBStringChars(mb_strcut($iso2022jpms, 0, 3, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 0, 4, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 0, 5, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 0, 6, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 0, 7, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 0, 8, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 0, 9, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";

print MBStringChars(mb_strcut($iso2022jpms, 1, 3, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 1, 6, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 1, 8, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 1, 9, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";

print MBStringChars(mb_strcut($iso2022jpms, 2, 5, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 5, 9, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 5, 11, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 6, 13, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 7, 13, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";

print MBStringChars(mb_strcut($iso2022jpms, 1, 100, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";
print MBStringChars(mb_strcut($iso2022jpms, 50, 100, 'ISO-2022-JP-MS'), 'ISO-2022-JP-MS') . "\n";

print "== JIS ==\n";
print MBStringChars(mb_strcut($jis, 0, 3, 'JIS'), 'JIS') . "\n";
print MBStringChars(mb_strcut($jis, 0, 4, 'JIS'), 'JIS') . "\n";
print MBStringChars(mb_strcut($jis, 0, 5, 'JIS'), 'JIS') . "\n";
print MBStringChars(mb_strcut($jis, 0, 6, 'JIS'), 'JIS') . "\n";
print MBStringChars(mb_strcut($jis, 0, 7, 'JIS'), 'JIS') . "\n";
print MBStringChars(mb_strcut($jis, 0, 8, 'JIS'), 'JIS') . "\n";

print MBStringChars(mb_strcut($jis, 1, 3, 'JIS'), 'JIS') . "\n";
print MBStringChars(mb_strcut($jis, 1, 6, 'JIS'), 'JIS') . "\n";
print MBStringChars(mb_strcut($jis, 1, 8, 'JIS'), 'JIS') . "\n";

print MBStringChars(mb_strcut($jis, 2, 5, 'JIS'), 'JIS') . "\n";
print MBStringChars(mb_strcut($jis, 5, 9, 'JIS'), 'JIS') . "\n";
print MBStringChars(mb_strcut($jis, 5, 11, 'JIS'), 'JIS') . "\n";
print MBStringChars(mb_strcut($jis, 6, 13, 'JIS'), 'JIS') . "\n";
print MBStringChars(mb_strcut($jis, 7, 13, 'JIS'), 'JIS') . "\n";

print MBStringChars(mb_strcut($jis, 1, 100, 'JIS'), 'JIS') . "\n";
print MBStringChars(mb_strcut($jis, 50, 100, 'JIS'), 'JIS') . "\n";

print "0xA3: [" . bin2hex(mb_strcut("\xA3aaaaaa", 0, 100, 'JIS')) . "]\n";
print "Bad escape sequence followed by null byte: [" . bin2hex(mb_strcut("\x1b\x00", 1, 100, "JIS")) . "]\n";

print "== ISO-2022-JP-KDDI ==\n";
print MBStringChars(mb_strcut($iso2022jp_kddi, 0, 3, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";
print MBStringChars(mb_strcut($iso2022jp_kddi, 0, 4, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";
print MBStringChars(mb_strcut($iso2022jp_kddi, 0, 5, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";
print MBStringChars(mb_strcut($iso2022jp_kddi, 0, 6, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";
print MBStringChars(mb_strcut($iso2022jp_kddi, 0, 7, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";
print MBStringChars(mb_strcut($iso2022jp_kddi, 0, 8, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";

print MBStringChars(mb_strcut($iso2022jp_kddi, 1, 3, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";
print MBStringChars(mb_strcut($iso2022jp_kddi, 1, 6, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";
print MBStringChars(mb_strcut($iso2022jp_kddi, 1, 8, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";

print MBStringChars(mb_strcut($iso2022jp_kddi, 2, 5, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";
print MBStringChars(mb_strcut($iso2022jp_kddi, 5, 9, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";
print MBStringChars(mb_strcut($iso2022jp_kddi, 5, 11, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";
print MBStringChars(mb_strcut($iso2022jp_kddi, 6, 13, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";
print MBStringChars(mb_strcut($iso2022jp_kddi, 7, 13, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";

print MBStringChars(mb_strcut($iso2022jp_kddi, 1, 100, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";
print MBStringChars(mb_strcut($iso2022jp_kddi, 50, 100, 'ISO-2022-JP-KDDI'), 'ISO-2022-JP-KDDI') . "\n";

print "== CP50220 ==\n";

print "Single byte 0xFF: [" . bin2hex(mb_strcut("\xFF", 0, 100, 'CP50220')) . "]\n";
print "Double byte 0xFF: [" . bin2hex(mb_strcut("\xFF\xFF", 0, 100, 'CP50220')) . "]\n";
print "Sample string with multiple null bytes: [" . bin2hex(mb_strcut("\xCF\x00\x00\x00\x00\x00d\x00\x00", 0, 100, 'CP50220')) . "]\n";
print "Bad escape sequence preceded by bad bytes: [" . bin2hex(mb_strcut("\xFF\xFF\x1B\x00", 0, 100, 'CP50220')) . "]\n";
print "Good JISX 0208 sequence, but it won't fit in max number of bytes: [" . bin2hex(mb_strcut("\x1B\$BGV\x17", 0, 100, 'CP50220')) . "]\n";
print "Bad escape sequence followed by GR kana: [" . bin2hex(mb_strcut("\x1B\$\xAC\x13", 0, 100, 'CP50220')) . "]\n";

print "== UTF-7 ==\n";

print "Single byte 0x01: [" . mb_strcut("\x01", 0, 100, 'UTF-7') . "]\n";
print "UTF-16 section ends abruptly: [" . mb_strcut("+Q", 1, 100, 'UTF-7') . "]\n";
print "UTF-16 section ends abruptly in middle of 2nd codepoint: [" . mb_strcut("+QxxC", 0, 100, 'UTF-7') . "]\n";
print "Cutting in middle of UTF-16 section: [" . mb_strcut("+UUU", -1, 255, "UTF-7") . "]\n";
print "Cutting in middle of UTF-16 section (2): [" . mb_strcut("+UUUU", -2, 255, "UTF-7") . "]\n";

print "== UTF7-IMAP ==\n";

print "Single byte 0x01: [" . mb_strcut("\x01", 0, 100, 'UTF7-IMAP') . "]\n";
print "UTF-16 section ends abruptly: [" . mb_strcut("&Q", 1, 100, 'UTF7-IMAP') . "]\n";
print "UTF-16 section ends abruptly in middle of 2nd codepoint: [" . mb_strcut("&QxxC", 0, 100, 'UTF7-IMAP') . "]\n";
print "UTF-16 section is terminated improperly: [" . mb_strcut("&i6o\x83", 0, 100, 'UTF7-IMAP') . "]\n";

print "== GB18030 ==\n";

print "Invalid byte 0xF5: [" . bin2hex(mb_strcut("\xF5a", 1, 100, 'GB18030')) . "]\n";
print "Double-byte char: [" . bin2hex(mb_strcut("\xAFw", -1, 100, "GB18030")) . "]\n";

print "== UHC ==\n";

print "Single byte 0x96: [" . bin2hex(mb_strcut("\x96", 1, 1280, "UHC")) . "]\n";

?>
--EXPECT--
== EUC-JP ==
[a4ce cab8]
[a4b3 a4ce]
[30 31 32 33 a4b3 a4ce cab8 bbfa cef3 a4cf c6fc cbdc b8ec a4c7 a4b9 a1a3 45 55 43 2d 4a 50 a4f2 bbc8 a4c3 a4c6 a4a4 a4de a4b9 a1a3 c6fc cbdc b8ec a4cf cccc c5dd bdad a4a4 a1a3]
OK
OK
== UTF-8 ==
[]
[]
[]
[e288ae]
[e288ae 20]
[e288ae 20 45]
[]
[e288ae]
[e288ae 20]
[c3a5 42 c3a4 43 c3b6 44 c3bc]
== UTF-16 ==
Single byte: []
With from=1: []
Bad surrogate: []
Bad surrogate followed by other bytes: [003f1243]
BE byte order mark: []
LE byte order mark: []
Length=0: []
== UTF-16LE ==
[]
[]
[1a04]
[1a04]
[1a04]
[1a04]
[1a04 3804]
Single byte: []
== UTF-32BE ==
[]
[0000222e]
[0000222e]
[0000222e 00000020]
[0000222e 00000020]
== ISO-2022-JP ==
[]
[]
[]
[]
[]
[1b244234411b2842]
[]
[]
[1b244234411b2842]
[]
[1b24423b7a1b2842 20]
[1b24423b7a1b2842 20 61 62]
[1b24423b7a1b2842 20 61 62 63 20]
[20 61 62 63 20 1b2442252b1b2842]
[1b244234411b2842 1b24423b7a1b2842 20 61 62 63 20 1b2442252b1b2842]
[]
Error followed by ASCII char: [4b]
== ISO-2022-JP-2004 ==
[]
[]
[]
[]
[]
[]
[1b24285134411b2842]
[]
[]
[]
[1b24285134411b2842]
[]
[1b24285134411b2842]
[1b24285134411b2842 1b2428513b7a1b2842]
[1b2428513b7a1b2842 20 61 62 63]
[1b2428513b7a1b2842 20 61 62 63]
[1b24285134411b2842 1b2428513b7a1b2842 20 61 62 63 20 1b242851252b1b2842 1b242851254a1b2842]
[]
== ISO-2022-JP-MS ==
[]
[]
[]
[]
[]
[1b244234411b2842]
[1b244234411b2842]
[]
[]
[1b244234411b2842]
[1b244234411b2842]
[]
[1b24423b7a1b2842 20]
[1b24423b7a1b2842 20 61 62]
[1b24423b7a1b2842 20 61 62 63 20]
[20 61 62 63 20 1b2442252b1b2842]
[1b244234411b2842 1b24423b7a1b2842 20 61 62 63 20 1b2442252b1b2842]
[]
== JIS ==
[]
[]
[]
[]
[]
[1b244234411b2842]
[]
[]
[1b244234411b2842]
[]
[1b24423b7a1b2842 20]
[1b24423b7a1b2842 20 61 62]
[1b24423b7a1b2842 20 61 62 63 20]
[20 61 62 63 20 1b2442252b1b2842]
[1b244234411b2842 1b24423b7a1b2842 20 61 62 63 20 1b2442252b1b2842]
[]
0xA3: []
Bad escape sequence followed by null byte: []
== ISO-2022-JP-KDDI ==
[]
[]
[]
[]
[]
[1b244234411b2842]
[]
[]
[1b244234411b2842]
[]
[1b24423b7a1b2842 20]
[1b24423b7a1b2842 20 61 62]
[1b24423b7a1b2842 20 61 62 63 20]
[20 61 62 63 20 1b2442252b1b2842]
[1b244234411b2842 1b24423b7a1b2842 20 61 62 63 20 1b2442252b1b2842]
[]
== CP50220 ==
Single byte 0xFF: []
Double byte 0xFF: [3f]
Sample string with multiple null bytes: [1b2442255e001b2842]
Bad escape sequence preceded by bad bytes: [3f3f3f00]
Good JISX 0208 sequence, but it won't fit in max number of bytes: []
Bad escape sequence followed by GR kana: []
== UTF-7 ==
Single byte 0x01: []
UTF-16 section ends abruptly: []
UTF-16 section ends abruptly in middle of 2nd codepoint: [+Qxw-]
Cutting in middle of UTF-16 section: []
Cutting in middle of UTF-16 section (2): []
== UTF7-IMAP ==
Single byte 0x01: [?]
UTF-16 section ends abruptly: []
UTF-16 section ends abruptly in middle of 2nd codepoint: []
UTF-16 section is terminated improperly: []
== GB18030 ==
Invalid byte 0xF5: []
Double-byte char: []
== UHC ==
Single byte 0x96: [96]
