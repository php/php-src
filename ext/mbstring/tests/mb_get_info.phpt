--TEST--
Test mb_get_info() function
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip'); ?>
--INI--
mbstring.encoding_translation=1
mbstring.language=Korean
mbstring.internal_encoding=UTF-8
mbstring.http_input=ISO-8859-1
mbstring.http_output=ISO-8859-15
mbstring.http_output_conv_mimetypes=abc
mbstring.func_overload=2
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
array(15) {
  ["internal_encoding"]=>
  string(5) "UTF-8"
  ["http_input"]=>
  string(10) "ISO-8859-1"
  ["http_output"]=>
  string(11) "ISO-8859-15"
  ["http_output_conv_mimetypes"]=>
  string(3) "abc"
  ["func_overload"]=>
  int(2)
  ["func_overload_list"]=>
  array(12) {
    ["strlen"]=>
    string(9) "mb_strlen"
    ["strpos"]=>
    string(9) "mb_strpos"
    ["strrpos"]=>
    string(10) "mb_strrpos"
    ["stripos"]=>
    string(10) "mb_stripos"
    ["strripos"]=>
    string(11) "mb_strripos"
    ["strstr"]=>
    string(9) "mb_strstr"
    ["strrchr"]=>
    string(10) "mb_strrchr"
    ["stristr"]=>
    string(10) "mb_stristr"
    ["substr"]=>
    string(9) "mb_substr"
    ["strtolower"]=>
    string(13) "mb_strtolower"
    ["strtoupper"]=>
    string(13) "mb_strtoupper"
    ["substr_count"]=>
    string(15) "mb_substr_count"
  }
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
int(2)
int(2)
array(12) {
  ["strlen"]=>
  string(9) "mb_strlen"
  ["strpos"]=>
  string(9) "mb_strpos"
  ["strrpos"]=>
  string(10) "mb_strrpos"
  ["stripos"]=>
  string(10) "mb_stripos"
  ["strripos"]=>
  string(11) "mb_strripos"
  ["strstr"]=>
  string(9) "mb_strstr"
  ["strrchr"]=>
  string(10) "mb_strrchr"
  ["stristr"]=>
  string(10) "mb_stristr"
  ["substr"]=>
  string(9) "mb_substr"
  ["strtolower"]=>
  string(13) "mb_strtolower"
  ["strtoupper"]=>
  string(13) "mb_strtoupper"
  ["substr_count"]=>
  string(15) "mb_substr_count"
}
array(12) {
  ["strlen"]=>
  string(9) "mb_strlen"
  ["strpos"]=>
  string(9) "mb_strpos"
  ["strrpos"]=>
  string(10) "mb_strrpos"
  ["stripos"]=>
  string(10) "mb_stripos"
  ["strripos"]=>
  string(11) "mb_strripos"
  ["strstr"]=>
  string(9) "mb_strstr"
  ["strrchr"]=>
  string(10) "mb_strrchr"
  ["stristr"]=>
  string(10) "mb_stristr"
  ["substr"]=>
  string(9) "mb_substr"
  ["strtolower"]=>
  string(13) "mb_strtolower"
  ["strtoupper"]=>
  string(13) "mb_strtoupper"
  ["substr_count"]=>
  string(15) "mb_substr_count"
}
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
