--TEST--
Surrogate codepoints are not encoded as UTF-8
--EXTENSIONS--
mbstring
--FILE--
<?php
mb_internal_encoding('UTF-8');

function test(string $desc, string $str) {
    echo $desc, ': ', bin2hex($str), ' ', var_export(mb_check_encoding($str, 'UTF-8'), true), "\n";
}

test('UCS-4', mb_convert_encoding("\x00\x00\x00A\x00\x00\xD8\x00\x00\x00\x00B", 'UTF-8', 'UCS-4'));
test('UCS-4LE', mb_convert_encoding("\xFF\xDF\x00\x00", 'UTF-8', 'UCS-4LE'));
test('UCS-2', mb_convert_encoding("\xD8\x3D\xDE\x00", 'UTF-8', 'UCS-2'));
test('UCS-2LE', mb_convert_encoding("\x00\xDC", 'UTF-8', 'UCS-2LE'));
test('HTML-ENTITIES', mb_convert_encoding('&#xD800;&#57343;', 'UTF-8', 'HTML-ENTITIES'));

$vars = ["\xD8\x00"];
mb_convert_variables('UTF-8', 'UCS-2', $vars);
test('mb_convert_variables', $vars[0]);

test('mb_decode_numericentity', mb_decode_numericentity('&#55296;&#xDFFF;', [0, 0x10FFFF, 0, 0x1FFFFF], 'UTF-8'));
test('mb_decode_mimeheader', mb_decode_mimeheader('=?UCS-2?B?2AA=?='));

mb_substitute_character('long');
test('long', mb_convert_encoding("\x00\x00\xD8\x00", 'UTF-8', 'UCS-4'));

// Searching still tells surrogates apart
$haystack = "\xD8\x3D\xDE\x00\x00A\xD8\x3D\xDE\x01";
var_dump(mb_strpos($haystack, "\xD8\x3D\xDE\x01", 0, 'UCS-2'));
var_dump(mb_stripos($haystack, "\xD8\x3D\xDE\x01", 0, 'UCS-2'));
var_dump(mb_substr_count($haystack, "\xDE\x01", 'UCS-2'));
?>
--EXPECT--
UCS-4: 413f42 true
UCS-4LE: 3f true
UCS-2: 3f3f true
UCS-2LE: 3f true
HTML-ENTITIES: 3f3f true
mb_convert_variables: 3f true
mb_decode_numericentity: 3f3f true
mb_decode_mimeheader: 3f true
long: 552b44383030 true
int(3)
int(3)
int(1)
