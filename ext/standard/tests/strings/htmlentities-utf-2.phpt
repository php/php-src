--TEST--
HTML entities with invalid chars and ENT_IGNORE
--INI--
output_handler=
--FILE--
<?php
@setlocale (LC_CTYPE, "C");
$strings = array("<", "\xD0", "\xD0\x90", "\xD0\x90\xD0", "\xD0\x90\xD0\xB0", "\xE0", "A\xE0", "\xE0\x80", "\xE0\x79", "\xE0\x80\xBE",
	"Voil\xE0", "Clich\xE9s",
	"\xFE", "\xFE\x41", "\xC3\xA9", "\xC3\x79", "\xF7\xBF\xBF\xBF", "\xFB\xBF\xBF\xBF\xBF", "\xFD\xBF\xBF\xBF\xBF\xBF",
	"\x41\xF7\xF7\x42", "\x42\xFB\xFB\x42", "\x43\xFD\xFD\x42", "\x44\xF7\xF7", "\x45\xFB\xFB", "\x46\xFD\xFD"
	);
foreach($strings as $string) {
	$sc_encoded = htmlspecialchars ($string, ENT_QUOTES | ENT_IGNORE, "utf-8");
	var_dump(bin2hex($sc_encoded));
	$ent_encoded = htmlentities ($string, ENT_QUOTES | ENT_IGNORE, "utf-8");
	var_dump(bin2hex($ent_encoded));
}
?>
--EXPECTF--
string(8) "266c743b"
string(8) "266c743b"
string(0) ""
string(0) ""
string(4) "d090"
string(4) "d090"
string(4) "d090"
string(4) "d090"
string(8) "d090d0b0"
string(8) "d090d0b0"
string(0) ""
string(0) ""
string(2) "41"
string(2) "41"
string(0) ""
string(0) ""
string(2) "79"
string(2) "79"
string(0) ""
string(0) ""
string(8) "566f696c"
string(8) "566f696c"
string(12) "436c69636873"
string(12) "436c69636873"
string(0) ""
string(0) ""
string(2) "41"
string(2) "41"
string(4) "c3a9"
string(16) "266561637574653b"
string(2) "79"
string(2) "79"
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(4) "4142"
string(4) "4142"
string(4) "4242"
string(4) "4242"
string(4) "4342"
string(4) "4342"
string(2) "44"
string(2) "44"
string(2) "45"
string(2) "45"
string(2) "46"
string(2) "46"
