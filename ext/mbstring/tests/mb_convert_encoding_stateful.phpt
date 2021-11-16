--TEST--
mb_convert_encoding() with stateful encodings
--EXTENSIONS--
mbstring
--INI--
output_handler=
mbstring.language=Japanese
--FILE--
<?php
echo "ISO-2022-JP empty segment\n";
echo bin2hex(mb_convert_encoding(pack("H*", "1b24401b24402121"), "UTF-8", "ISO-2022-JP")), "\n";
echo bin2hex(mb_convert_encoding(pack("H*", "1b24421b24422121"), "UTF-8", "ISO-2022-JP")), "\n";
echo bin2hex(mb_convert_encoding(pack("H*", "1b28421b284261626364"), "UTF-8", "ISO-2022-JP")), "\n";
echo bin2hex(mb_convert_encoding(pack("H*", "1b284a1b284a61626364"), "UTF-8", "ISO-2022-JP")), "\n";
echo bin2hex(mb_convert_encoding(pack("H*", "1b24401b284261626364"), "UTF-8", "ISO-2022-JP")), "\n";
echo bin2hex(mb_convert_encoding(pack("H*", "1b24401b284a61626364"), "UTF-8", "ISO-2022-JP")), "\n";
echo bin2hex(mb_convert_encoding(pack("H*", "1b24421b284261626364"), "UTF-8", "ISO-2022-JP")), "\n";
echo bin2hex(mb_convert_encoding(pack("H*", "1b24421b284a61626364"), "UTF-8", "ISO-2022-JP")), "\n";
echo "ISO-2022-KR empty segment\n";
echo bin2hex(mb_convert_encoding(pack("H*", "1b2429430e0f61626364"), "UTF-8", "ISO-2022-KR")), "\n";
echo "HZ empty segment\n";
echo bin2hex(mb_convert_encoding(pack("H*", "7e7b7e7d61626364"), "UTF-8", "HZ")), "\n";
?>
--EXPECT--
ISO-2022-JP empty segment
e38080
e38080
61626364
61626364
61626364
61626364
61626364
61626364
ISO-2022-KR empty segment
61626364
HZ empty segment
61626364
