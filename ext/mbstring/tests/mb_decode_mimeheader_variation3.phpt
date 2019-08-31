--TEST--
Test mb_decode_mimeheader() function : variation
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_decode_mimeheader') or die("skip mb_decode_mimeheader() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_decode_mimeheader(string string)
 * Description: Decodes the MIME "encoded-word" in the string
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions:
 */

echo "*** Testing mb_decode_mimeheader() : variation ***\n";
mb_internal_encoding('iso-8859-7');

//greek in UTF-8 to be converted to iso-8859-7
$encoded_word = "=?UTF-8?B?zrHOss6zzrTOtc62zrfOuM65zrrOu868zr3Ovs6/z4DPgc+Dz4TPhc+Gz4fPiM+J?=";
var_dump(bin2hex(mb_decode_mimeheader($encoded_word)));


?>
===DONE===
--EXPECT--
*** Testing mb_decode_mimeheader() : variation ***
string(48) "e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f3f4f5f6f7f8f9"
===DONE===
