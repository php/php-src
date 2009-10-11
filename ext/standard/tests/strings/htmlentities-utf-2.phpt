--TEST--
HTML entities with invalid chars and ENT_IGNORE
--INI--
output_handler=
--FILE--
<?php 
@setlocale (LC_CTYPE, "C");
$strings = array(b"<", b"\xD0", b"\xD0\x90", b"\xD0\x90\xD0", b"\xD0\x90\xD0\xB0", b"\xE0", b"A\xE0", b"\xE0\x80", b"\xE0\x79", b"\xE0\x80\xBE", 
	b"Voil\xE0", b"Clich\xE9s",
	b"\xFE", b"\xFE\x41", b"\xC3\xA9", b"\xC3\x79", b"\xF7\xBF\xBF\xBF", b"\xFB\xBF\xBF\xBF\xBF", b"\xFD\xBF\xBF\xBF\xBF\xBF",
	b"\x41\xF7\xF7\x42", b"\x42\xFB\xFB\x42", b"\x43\xFD\xFD\x42", b"\x44\xF7\xF7", b"\x45\xFB\xFB", b"\x46\xFD\xFD"
	);
foreach($strings as $string) {
	$sc_encoded = htmlspecialchars ($string, ENT_QUOTES | ENT_IGNORE, "utf-8");
	var_dump(bin2hex($sc_encoded));
	$ent_encoded = htmlentities ($string, ENT_QUOTES | ENT_IGNORE, "utf-8");
	var_dump(bin2hex($ent_encoded));
}
?>
--EXPECTF--
%unicode|string%(8) "266c743b"
%unicode|string%(8) "266c743b"
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(4) "d090"
%unicode|string%(4) "d090"
%unicode|string%(4) "d090"
%unicode|string%(4) "d090"
%unicode|string%(8) "d090d0b0"
%unicode|string%(8) "d090d0b0"
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(2) "41"
%unicode|string%(2) "41"
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(2) "79"
%unicode|string%(2) "79"
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(8) "566f696c"
%unicode|string%(8) "566f696c"
%unicode|string%(12) "436c69636873"
%unicode|string%(12) "436c69636873"
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(2) "41"
%unicode|string%(2) "41"
%unicode|string%(4) "c3a9"
%unicode|string%(16) "266561637574653b"
%unicode|string%(2) "79"
%unicode|string%(2) "79"
%unicode|string%(8) "f7bfbfbf"
%unicode|string%(8) "f7bfbfbf"
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(4) "4142"
%unicode|string%(4) "4142"
%unicode|string%(4) "4242"
%unicode|string%(4) "4242"
%unicode|string%(4) "4342"
%unicode|string%(4) "4342"
%unicode|string%(2) "44"
%unicode|string%(2) "44"
%unicode|string%(2) "45"
%unicode|string%(2) "45"
%unicode|string%(2) "46"
%unicode|string%(2) "46"
