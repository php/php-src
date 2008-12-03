--TEST--
HTML entities with invalid chars
--INI--
output_handler=
--FILE--
<?php 
setlocale (LC_CTYPE, "C");
$strings = array("<", "\xD0", "\xD0\x90", "\xD0\x90\xD0", "\xD0\x90\xD0\xB0", "\xE0", "A\xE0", "\xE0\x80", "\xE0\x80\xBE");
foreach($strings as $string) {
	$sc_encoded = htmlspecialchars ($string, ENT_QUOTES, "utf-8");
	var_dump(bin2hex($sc_encoded));
	$ent_encoded = htmlentities ($string, ENT_QUOTES, "utf-8");
	var_dump(bin2hex($ent_encoded));
}
?>
--EXPECT--
string(8) "266c743b"
string(8) "266c743b"
string(0) ""
string(0) ""
string(4) "d090"
string(4) "d090"
string(0) ""
string(0) ""
string(8) "d090d0b0"
string(8) "d090d0b0"
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(8) "2667743b"
string(8) "2667743b"