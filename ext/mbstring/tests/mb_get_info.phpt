--TEST--
Test mb_get_info() function
--EXTENSIONS--
mbstring
--INI--
mbstring.encoding_translation=1
mbstring.language=Korean
mbstring.internal_encoding=UTF-8
mbstring.http_input=ISO-8859-1
mbstring.http_output=ISO-8859-15
mbstring.http_output_conv_mimetypes=abc
mbstring.detect_order=UTF-8,ISO-8859-15,ISO-8859-1,ASCII
mbstring.substitute_character=123
mbstring.strict_detection=1
--FILE--
<?php
mb_parse_str("abc=def", $dummy);
mb_convert_encoding("\xff\xff", "Shift_JIS", "UCS-2BE");
$result = mb_get_info();
var_dump($result);
foreach (array_keys($result) as $key) {
    var_dump($result[$key], mb_get_info($key));
}
?>
--EXPECT--
Deprecated: PHP Startup: Use of mbstring.http_input is deprecated in Unknown on line 0

Deprecated: PHP Startup: Use of mbstring.http_output is deprecated in Unknown on line 0

Deprecated: PHP Startup: Use of mbstring.internal_encoding is deprecated in Unknown on line 0
array(13) {
  ["internal_encoding"]=>
  string(5) "UTF-8"
  ["http_input"]=>
  string(10) "ISO-8859-1"
  ["http_output"]=>
  string(11) "ISO-8859-15"
  ["http_output_conv_mimetypes"]=>
  string(3) "abc"
  ["mail_charset"]=>
  string(11) "ISO-2022-KR"
  ["mail_header_encoding"]=>
  string(6) "BASE64"
  ["mail_body_encoding"]=>
  string(4) "7bit"
  ["illegal_chars"]=>
  int(1)
  ["encoding_translation"]=>
  string(2) "On"
  ["language"]=>
  string(6) "Korean"
  ["detect_order"]=>
  array(4) {
    [0]=>
    string(5) "UTF-8"
    [1]=>
    string(11) "ISO-8859-15"
    [2]=>
    string(10) "ISO-8859-1"
    [3]=>
    string(5) "ASCII"
  }
  ["substitute_character"]=>
  int(123)
  ["strict_detection"]=>
  string(2) "On"
}
string(5) "UTF-8"
string(5) "UTF-8"
string(10) "ISO-8859-1"
string(10) "ISO-8859-1"
string(11) "ISO-8859-15"
string(11) "ISO-8859-15"
string(3) "abc"
string(3) "abc"
string(11) "ISO-2022-KR"
string(11) "ISO-2022-KR"
string(6) "BASE64"
string(6) "BASE64"
string(4) "7bit"
string(4) "7bit"
int(1)
int(1)
string(2) "On"
string(2) "On"
string(6) "Korean"
string(6) "Korean"
array(4) {
  [0]=>
  string(5) "UTF-8"
  [1]=>
  string(11) "ISO-8859-15"
  [2]=>
  string(10) "ISO-8859-1"
  [3]=>
  string(5) "ASCII"
}
array(4) {
  [0]=>
  string(5) "UTF-8"
  [1]=>
  string(11) "ISO-8859-15"
  [2]=>
  string(10) "ISO-8859-1"
  [3]=>
  string(5) "ASCII"
}
int(123)
int(123)
string(2) "On"
string(2) "On"
