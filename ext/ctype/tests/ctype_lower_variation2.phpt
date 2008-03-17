--TEST--
Test ctype_lower() function : usage variations - different integers
--SKIPIF--
<?php
if( ini_get("unicode.semantics") == "1") {
    die('skip do not run when unicode on');
}
?>
--FILE--
<?php
/* Prototype  : bool ctype_lower(mixed $c)
 * Description: Checks for lowercase character(s)  
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass different integers to ctype_lower() to test which character codes are considered
 * valid lowercase characters
 */

echo "*** Testing ctype_lower() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

for ($i = 0; $i < 256; $i++) {
	if (ctype_lower($i)) {
		echo "character code $i is a lower case character\n";
	}
}
 
setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_lower() : usage variations ***
character code 97 is a lower case character
character code 98 is a lower case character
character code 99 is a lower case character
character code 100 is a lower case character
character code 101 is a lower case character
character code 102 is a lower case character
character code 103 is a lower case character
character code 104 is a lower case character
character code 105 is a lower case character
character code 106 is a lower case character
character code 107 is a lower case character
character code 108 is a lower case character
character code 109 is a lower case character
character code 110 is a lower case character
character code 111 is a lower case character
character code 112 is a lower case character
character code 113 is a lower case character
character code 114 is a lower case character
character code 115 is a lower case character
character code 116 is a lower case character
character code 117 is a lower case character
character code 118 is a lower case character
character code 119 is a lower case character
character code 120 is a lower case character
character code 121 is a lower case character
character code 122 is a lower case character
character code 170 is a lower case character
character code 181 is a lower case character
character code 186 is a lower case character
character code 223 is a lower case character
character code 224 is a lower case character
character code 225 is a lower case character
character code 226 is a lower case character
character code 227 is a lower case character
character code 228 is a lower case character
character code 229 is a lower case character
character code 230 is a lower case character
character code 231 is a lower case character
character code 232 is a lower case character
character code 233 is a lower case character
character code 234 is a lower case character
character code 235 is a lower case character
character code 236 is a lower case character
character code 237 is a lower case character
character code 238 is a lower case character
character code 239 is a lower case character
character code 240 is a lower case character
character code 241 is a lower case character
character code 242 is a lower case character
character code 243 is a lower case character
character code 244 is a lower case character
character code 245 is a lower case character
character code 246 is a lower case character
character code 248 is a lower case character
character code 249 is a lower case character
character code 250 is a lower case character
character code 251 is a lower case character
character code 252 is a lower case character
character code 253 is a lower case character
character code 254 is a lower case character
character code 255 is a lower case character
===DONE===
