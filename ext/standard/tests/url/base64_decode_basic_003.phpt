--TEST--
Test base64_decode() function : basic functionality - padding and whitespace
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
/* Prototype  : proto string base64_decode(string str[, bool strict])
 * Description: Decodes string using MIME base64 algorithm
 * Source code: ext/standard/base64.c
 * Alias to functions:
 */

echo "Test base64_decode (output as JSON):\n";
$data = [
	"", "=", "==", "===", "====",
	"V", "V=", "V==", "V===", "V====",
	"VV", "VV=", "VV==", "VV===", "VV====",
	"VVV", "VVV=", "VVV==", "VVV===", "VVV====",
	"VVVV", "VVVV=", "VVVV==", "VVVV===", "VVVV====",
	"=V", "=VV", "=VVV",
	"==V", "==VV", "==VVV",
	"===V", "===VV", "===VVV",
	"====V", "====VV", "====VVV",
	"=VVV", "V=VV", "VV=V", "VVV=",
	"=VVVV", "V=VVV", "VV=VV", "VVV=V", "VVVV=",
	"=VVV=", "V=VV=", "VV=V=", "VVV==",
	"\nVV", "V\nV", "VV\n",
	"\nVV==", "V\nV==", "VV\n==", "VV=\n=", "VV==\n",
	"*VV", "V*V", "VV*",
	"*VV==", "V*V==", "VV*==", "VV=*=", "VV==*",
	"\0VV==", "V\0V==", "VV\0==", "VV=\0=", "VV==\0",
	"\0VVV==", "V\0VV==", "VV\0V==", "VVV\0==", "VVV=\0=", "VVV==\0",
];
foreach ($data as $a) {
	$b = base64_decode($a, false);
	$c = base64_decode($a, true);
	printf("base64 %-16s non-strict %-8s strict %s\n", json_encode($a), json_encode($b), json_encode($c));
}
echo "Done\n";
?>
--EXPECTF--
Test base64_decode (output as JSON):
base64 ""               non-strict ""       strict ""
base64 "="              non-strict ""       strict false
base64 "=="             non-strict ""       strict false
base64 "==="            non-strict ""       strict false
base64 "===="           non-strict ""       strict false
base64 "V"              non-strict ""       strict false
base64 "V="             non-strict ""       strict false
base64 "V=="            non-strict ""       strict false
base64 "V==="           non-strict ""       strict false
base64 "V===="          non-strict ""       strict false
base64 "VV"             non-strict "U"      strict "U"
base64 "VV="            non-strict "U"      strict false
base64 "VV=="           non-strict "U"      strict "U"
base64 "VV==="          non-strict "U"      strict false
base64 "VV===="         non-strict "U"      strict false
base64 "VVV"            non-strict "UU"     strict "UU"
base64 "VVV="           non-strict "UU"     strict "UU"
base64 "VVV=="          non-strict "UU"     strict false
base64 "VVV==="         non-strict "UU"     strict false
base64 "VVV===="        non-strict "UU"     strict false
base64 "VVVV"           non-strict "UUU"    strict "UUU"
base64 "VVVV="          non-strict "UUU"    strict false
base64 "VVVV=="         non-strict "UUU"    strict false
base64 "VVVV==="        non-strict "UUU"    strict false
base64 "VVVV===="       non-strict "UUU"    strict false
base64 "=V"             non-strict ""       strict false
base64 "=VV"            non-strict "U"      strict false
base64 "=VVV"           non-strict "UU"     strict false
base64 "==V"            non-strict ""       strict false
base64 "==VV"           non-strict "U"      strict false
base64 "==VVV"          non-strict "UU"     strict false
base64 "===V"           non-strict ""       strict false
base64 "===VV"          non-strict "U"      strict false
base64 "===VVV"         non-strict "UU"     strict false
base64 "====V"          non-strict ""       strict false
base64 "====VV"         non-strict "U"      strict false
base64 "====VVV"        non-strict "UU"     strict false
base64 "=VVV"           non-strict "UU"     strict false
base64 "V=VV"           non-strict "UU"     strict false
base64 "VV=V"           non-strict "UU"     strict false
base64 "VVV="           non-strict "UU"     strict "UU"
base64 "=VVVV"          non-strict "UUU"    strict false
base64 "V=VVV"          non-strict "UUU"    strict false
base64 "VV=VV"          non-strict "UUU"    strict false
base64 "VVV=V"          non-strict "UUU"    strict false
base64 "VVVV="          non-strict "UUU"    strict false
base64 "=VVV="          non-strict "UU"     strict false
base64 "V=VV="          non-strict "UU"     strict false
base64 "VV=V="          non-strict "UU"     strict false
base64 "VVV=="          non-strict "UU"     strict false
base64 "\nVV"           non-strict "U"      strict "U"
base64 "V\nV"           non-strict "U"      strict "U"
base64 "VV\n"           non-strict "U"      strict "U"
base64 "\nVV=="         non-strict "U"      strict "U"
base64 "V\nV=="         non-strict "U"      strict "U"
base64 "VV\n=="         non-strict "U"      strict "U"
base64 "VV=\n="         non-strict "U"      strict "U"
base64 "VV==\n"         non-strict "U"      strict "U"
base64 "*VV"            non-strict "U"      strict false
base64 "V*V"            non-strict "U"      strict false
base64 "VV*"            non-strict "U"      strict false
base64 "*VV=="          non-strict "U"      strict false
base64 "V*V=="          non-strict "U"      strict false
base64 "VV*=="          non-strict "U"      strict false
base64 "VV=*="          non-strict "U"      strict false
base64 "VV==*"          non-strict "U"      strict false
base64 "\u0000VV=="     non-strict "U"      strict false
base64 "V\u0000V=="     non-strict "U"      strict false
base64 "VV\u0000=="     non-strict "U"      strict false
base64 "VV=\u0000="     non-strict "U"      strict false
base64 "VV==\u0000"     non-strict "U"      strict false
base64 "\u0000VVV=="    non-strict "UU"     strict false
base64 "V\u0000VV=="    non-strict "UU"     strict false
base64 "VV\u0000V=="    non-strict "UU"     strict false
base64 "VVV\u0000=="    non-strict "UU"     strict false
base64 "VVV=\u0000="    non-strict "UU"     strict false
base64 "VVV==\u0000"    non-strict "UU"     strict false
Done
