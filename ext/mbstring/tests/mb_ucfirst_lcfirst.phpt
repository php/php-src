--TEST--
mb_ucfirst(), mb_lcfirst functions tests
--EXTENSIONS--
mbstring
--FILE--
<?php
mb_internal_encoding("UTF-8");

function test_ascii_mb_ucfirst() {
	for ($i = 0; $i < 128; $i++) {
		if ($i >= 97 && $i <= 122) { /* a to z */
			if (mb_ucfirst(chr($i)) !== chr($i - (97 - 65))) {
				echo "mb_ucfirst compare failed: " . chr($i) . "\n";
			}
		} else {
			if (mb_ucfirst(chr($i)) !== chr($i)) {
				echo "mb_ucfirst compare failed: " . chr($i) . "\n";
			}
		}
	}
	echo "Done mb_ucfirst\n";
}

function test_ascii_mb_lcfirst() {
	for ($i = 0; $i < 128; $i++) {
		if ($i >= 65 && $i <= 90) { /* A to Z */
			if (mb_lcfirst(chr($i)) !== chr($i + (97 - 65))) {
				echo "mb_lcfirst compare failed: " . chr($i) . "\n";
				return;
			}
		} else {
			if (mb_lcfirst(chr($i)) !== chr($i)) {
				echo "mb_lcfirst compare failed: " . chr($i) . "\n";
				return;
			}
		}
	}
	echo "Done mb_lcfirst\n";
}

echo "== Empty String ==\n";
var_dump(mb_ucfirst(""));
var_dump(mb_lcfirst(""));
echo "== ASCII ==\n";
test_ascii_mb_ucfirst();
test_ascii_mb_lcfirst();
echo "== mb_ucfirst ==\n";
var_dump(mb_ucfirst("ａｂ"));
var_dump(mb_ucfirst("ＡＢＳ"));
var_dump(mb_ucfirst("đắt quá!"));
var_dump(mb_ucfirst("აბგ"));
var_dump(mb_ucfirst("ǉ"));
echo "== mb_lcfirst ==\n";
var_dump(mb_lcfirst("ＡＢＳ"));
var_dump(mb_lcfirst("Xin chào"));
var_dump(mb_lcfirst("Đẹp quá!"));
echo "== SJIS ==\n";
var_dump(bin2hex(mb_ucfirst(mb_convert_encoding("ｅｂｉ", "SJIS", "UTF-8"), "SJIS")));
var_dump(bin2hex(mb_lcfirst(mb_convert_encoding("ＥＢＩ", "SJIS", "UTF-8"), "SJIS")));
var_dump(bin2hex(mb_ucfirst(hex2bin("8471"), "SJIS"))); /* б */
var_dump(bin2hex(mb_lcfirst(hex2bin("8441"), "SJIS"))); /* Б */
var_dump(bin2hex(mb_ucfirst(hex2bin("83bf"), "SJIS"))); /* α */
var_dump(bin2hex(mb_lcfirst(hex2bin("839f"), "SJIS"))); /* Α */
var_dump(bin2hex(mb_lcfirst(hex2bin("82a0"), "SJIS"))); /* あ */
var_dump(bin2hex(mb_ucfirst(hex2bin("83bf8471"), "SJIS")));
var_dump(bin2hex(mb_lcfirst(hex2bin("839f8441"), "SJIS")));
echo "== EUC-JP ==\n";
var_dump(bin2hex(mb_ucfirst(hex2bin("a6d8"), "EUC-JP"))); /* Ω */
var_dump(bin2hex(mb_lcfirst(hex2bin("a6b8"), "EUC-JP"))); /* ω */
var_dump(bin2hex(mb_ucfirst(hex2bin("a4a2a4a2"), "EUC-JP"))); /* あ */
echo "== Longer strings ==\n";
var_dump(mb_ucfirst("э" . str_repeat("A", 65536)) === "Э" . str_repeat("A", 65536));
var_dump(mb_lcfirst("Э" . str_repeat("A", 65536)) === "э" . str_repeat("A", 65536));
?>
--EXPECT--
== Empty String ==
string(0) ""
string(0) ""
== ASCII ==
Done mb_ucfirst
Done mb_lcfirst
== mb_ucfirst ==
string(6) "Ａｂ"
string(9) "ＡＢＳ"
string(12) "Đắt quá!"
string(9) "აბგ"
string(2) "ǈ"
== mb_lcfirst ==
string(9) "ａＢＳ"
string(9) "xin chào"
string(12) "đẹp quá!"
== SJIS ==
string(12) "826482828289"
string(12) "828582618268"
string(4) "8441"
string(4) "8471"
string(4) "839f"
string(4) "83bf"
string(4) "82a0"
string(8) "839f8471"
string(8) "83bf8441"
== EUC-JP ==
string(4) "a6b8"
string(4) "a6d8"
string(8) "a4a2a4a2"
== Longer strings ==
bool(true)
bool(true)
