--TEST--
Test ctype_alpha() function : usage variations - different integers
--SKIPIF--
<?php
if( ini_get("unicode.semantics") == "1") {
    die('skip do not run when unicode on');
}
?>
--FILE--
<?php
/* Prototype  : bool ctype_alpha(mixed $c)
 * Description: Checks for alphabetic character(s) 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass different integers to ctype_alpha() to test which character codes are considered
 * valid alphabetic characters
 */

echo "*** Testing ctype_alpha() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

for ($i = 0; $i < 256; $i++) {
	if (ctype_alpha($i)) {
		echo "character code $i is alphabetic\n";
	}
}

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_alpha() : usage variations ***
character code 65 is alphabetic
character code 66 is alphabetic
character code 67 is alphabetic
character code 68 is alphabetic
character code 69 is alphabetic
character code 70 is alphabetic
character code 71 is alphabetic
character code 72 is alphabetic
character code 73 is alphabetic
character code 74 is alphabetic
character code 75 is alphabetic
character code 76 is alphabetic
character code 77 is alphabetic
character code 78 is alphabetic
character code 79 is alphabetic
character code 80 is alphabetic
character code 81 is alphabetic
character code 82 is alphabetic
character code 83 is alphabetic
character code 84 is alphabetic
character code 85 is alphabetic
character code 86 is alphabetic
character code 87 is alphabetic
character code 88 is alphabetic
character code 89 is alphabetic
character code 90 is alphabetic
character code 97 is alphabetic
character code 98 is alphabetic
character code 99 is alphabetic
character code 100 is alphabetic
character code 101 is alphabetic
character code 102 is alphabetic
character code 103 is alphabetic
character code 104 is alphabetic
character code 105 is alphabetic
character code 106 is alphabetic
character code 107 is alphabetic
character code 108 is alphabetic
character code 109 is alphabetic
character code 110 is alphabetic
character code 111 is alphabetic
character code 112 is alphabetic
character code 113 is alphabetic
character code 114 is alphabetic
character code 115 is alphabetic
character code 116 is alphabetic
character code 117 is alphabetic
character code 118 is alphabetic
character code 119 is alphabetic
character code 120 is alphabetic
character code 121 is alphabetic
character code 122 is alphabetic
character code 170 is alphabetic
character code 181 is alphabetic
character code 186 is alphabetic
character code 192 is alphabetic
character code 193 is alphabetic
character code 194 is alphabetic
character code 195 is alphabetic
character code 196 is alphabetic
character code 197 is alphabetic
character code 198 is alphabetic
character code 199 is alphabetic
character code 200 is alphabetic
character code 201 is alphabetic
character code 202 is alphabetic
character code 203 is alphabetic
character code 204 is alphabetic
character code 205 is alphabetic
character code 206 is alphabetic
character code 207 is alphabetic
character code 208 is alphabetic
character code 209 is alphabetic
character code 210 is alphabetic
character code 211 is alphabetic
character code 212 is alphabetic
character code 213 is alphabetic
character code 214 is alphabetic
character code 216 is alphabetic
character code 217 is alphabetic
character code 218 is alphabetic
character code 219 is alphabetic
character code 220 is alphabetic
character code 221 is alphabetic
character code 222 is alphabetic
character code 223 is alphabetic
character code 224 is alphabetic
character code 225 is alphabetic
character code 226 is alphabetic
character code 227 is alphabetic
character code 228 is alphabetic
character code 229 is alphabetic
character code 230 is alphabetic
character code 231 is alphabetic
character code 232 is alphabetic
character code 233 is alphabetic
character code 234 is alphabetic
character code 235 is alphabetic
character code 236 is alphabetic
character code 237 is alphabetic
character code 238 is alphabetic
character code 239 is alphabetic
character code 240 is alphabetic
character code 241 is alphabetic
character code 242 is alphabetic
character code 243 is alphabetic
character code 244 is alphabetic
character code 245 is alphabetic
character code 246 is alphabetic
character code 248 is alphabetic
character code 249 is alphabetic
character code 250 is alphabetic
character code 251 is alphabetic
character code 252 is alphabetic
character code 253 is alphabetic
character code 254 is alphabetic
character code 255 is alphabetic
===DONE===
