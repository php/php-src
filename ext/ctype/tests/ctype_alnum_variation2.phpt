--TEST--
Test ctype_alnum() function : usage variations - different integers
--SKIPIF--
<?php
if( ini_get("unicode.semantics") == "1") {
    die('skip do not run when unicode on');
}
?>
--FILE--
<?php
/* Prototype  : bool ctype_alnum(mixed $c)
 * Description: Checks for alphanumeric character(s) 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass different integers to ctype_alnum() to test which character codes are considered
 * valid alphanumeric characters
 */

echo "*** Testing ctype_alnum() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C"); 

for ($i = 0; $i < 256; $i++) {
	if (ctype_alnum($i)) {
		echo "character code $i is alpha numeric\n";
	}
}

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_alnum() : usage variations ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_alnum_variation2.php on line 14
character code 48 is alpha numeric
character code 49 is alpha numeric
character code 50 is alpha numeric
character code 51 is alpha numeric
character code 52 is alpha numeric
character code 53 is alpha numeric
character code 54 is alpha numeric
character code 55 is alpha numeric
character code 56 is alpha numeric
character code 57 is alpha numeric
character code 65 is alpha numeric
character code 66 is alpha numeric
character code 67 is alpha numeric
character code 68 is alpha numeric
character code 69 is alpha numeric
character code 70 is alpha numeric
character code 71 is alpha numeric
character code 72 is alpha numeric
character code 73 is alpha numeric
character code 74 is alpha numeric
character code 75 is alpha numeric
character code 76 is alpha numeric
character code 77 is alpha numeric
character code 78 is alpha numeric
character code 79 is alpha numeric
character code 80 is alpha numeric
character code 81 is alpha numeric
character code 82 is alpha numeric
character code 83 is alpha numeric
character code 84 is alpha numeric
character code 85 is alpha numeric
character code 86 is alpha numeric
character code 87 is alpha numeric
character code 88 is alpha numeric
character code 89 is alpha numeric
character code 90 is alpha numeric
character code 97 is alpha numeric
character code 98 is alpha numeric
character code 99 is alpha numeric
character code 100 is alpha numeric
character code 101 is alpha numeric
character code 102 is alpha numeric
character code 103 is alpha numeric
character code 104 is alpha numeric
character code 105 is alpha numeric
character code 106 is alpha numeric
character code 107 is alpha numeric
character code 108 is alpha numeric
character code 109 is alpha numeric
character code 110 is alpha numeric
character code 111 is alpha numeric
character code 112 is alpha numeric
character code 113 is alpha numeric
character code 114 is alpha numeric
character code 115 is alpha numeric
character code 116 is alpha numeric
character code 117 is alpha numeric
character code 118 is alpha numeric
character code 119 is alpha numeric
character code 120 is alpha numeric
character code 121 is alpha numeric
character code 122 is alpha numeric
character code 170 is alpha numeric
character code 181 is alpha numeric
character code 186 is alpha numeric
character code 192 is alpha numeric
character code 193 is alpha numeric
character code 194 is alpha numeric
character code 195 is alpha numeric
character code 196 is alpha numeric
character code 197 is alpha numeric
character code 198 is alpha numeric
character code 199 is alpha numeric
character code 200 is alpha numeric
character code 201 is alpha numeric
character code 202 is alpha numeric
character code 203 is alpha numeric
character code 204 is alpha numeric
character code 205 is alpha numeric
character code 206 is alpha numeric
character code 207 is alpha numeric
character code 208 is alpha numeric
character code 209 is alpha numeric
character code 210 is alpha numeric
character code 211 is alpha numeric
character code 212 is alpha numeric
character code 213 is alpha numeric
character code 214 is alpha numeric
character code 216 is alpha numeric
character code 217 is alpha numeric
character code 218 is alpha numeric
character code 219 is alpha numeric
character code 220 is alpha numeric
character code 221 is alpha numeric
character code 222 is alpha numeric
character code 223 is alpha numeric
character code 224 is alpha numeric
character code 225 is alpha numeric
character code 226 is alpha numeric
character code 227 is alpha numeric
character code 228 is alpha numeric
character code 229 is alpha numeric
character code 230 is alpha numeric
character code 231 is alpha numeric
character code 232 is alpha numeric
character code 233 is alpha numeric
character code 234 is alpha numeric
character code 235 is alpha numeric
character code 236 is alpha numeric
character code 237 is alpha numeric
character code 238 is alpha numeric
character code 239 is alpha numeric
character code 240 is alpha numeric
character code 241 is alpha numeric
character code 242 is alpha numeric
character code 243 is alpha numeric
character code 244 is alpha numeric
character code 245 is alpha numeric
character code 246 is alpha numeric
character code 248 is alpha numeric
character code 249 is alpha numeric
character code 250 is alpha numeric
character code 251 is alpha numeric
character code 252 is alpha numeric
character code 253 is alpha numeric
character code 254 is alpha numeric
character code 255 is alpha numeric

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_alnum_variation2.php on line 22
===DONE===
