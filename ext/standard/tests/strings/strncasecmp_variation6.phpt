--TEST--
Test strncasecmp() function : usage variations - binary safe - all ASCII chars
--FILE--
<?php
/* Prototype  : int strncasecmp ( string $str1, string $str2, int $len );
 * Description: Binary safe case-insensitive string comparison of the first n characters
 * Source code: Zend/zend_builtin_functions.c
*/

/* Test strncasecmp() function with binary values passed to 'str1' & 'str2' */

echo "*** Test strncasecmp() function: with binary inputs ***\n";

/* A binary function should work with all 256 characters that a character(8-bit) can take */
echo "\n-- Checking with all 256 characters given, in binary format --\n";
/* loop through to get all 256 character's equivalent binary value, and check working of strncasecmp() */
$count = 1;
for($ASCII = 0; $ASCII <= 255; $ASCII++) {
  $str1 = decbin($ASCII);  //ASCII value in binary form
  $str2 = decbin( ord( chr($ASCII) ) );  //Getting equivalent ASCII value for the character in binary form
  echo "-- Iteration $count --\n";
  var_dump( strncasecmp($str1, $str2, 8) );  //comparing all the 8-bits; expected: int(0)
  var_dump( strncasecmp($str1, $str2, 4) );  //comparing only 4-bits; expected: int(0)
  $count++;
}

echo "\n-- Checking with out of character's range, given in binary format --\n";
$str1 = decbin(256);
$str2 = decbin( ord( chr(256) ));
var_dump( strncasecmp($str1, $str2, 8) );  //comparing all the 8-bits; expected: int(1)

echo "\n*** Done ***\n";
?>
--EXPECT--
*** Test strncasecmp() function: with binary inputs ***

-- Checking with all 256 characters given, in binary format --
-- Iteration 1 --
int(0)
int(0)
-- Iteration 2 --
int(0)
int(0)
-- Iteration 3 --
int(0)
int(0)
-- Iteration 4 --
int(0)
int(0)
-- Iteration 5 --
int(0)
int(0)
-- Iteration 6 --
int(0)
int(0)
-- Iteration 7 --
int(0)
int(0)
-- Iteration 8 --
int(0)
int(0)
-- Iteration 9 --
int(0)
int(0)
-- Iteration 10 --
int(0)
int(0)
-- Iteration 11 --
int(0)
int(0)
-- Iteration 12 --
int(0)
int(0)
-- Iteration 13 --
int(0)
int(0)
-- Iteration 14 --
int(0)
int(0)
-- Iteration 15 --
int(0)
int(0)
-- Iteration 16 --
int(0)
int(0)
-- Iteration 17 --
int(0)
int(0)
-- Iteration 18 --
int(0)
int(0)
-- Iteration 19 --
int(0)
int(0)
-- Iteration 20 --
int(0)
int(0)
-- Iteration 21 --
int(0)
int(0)
-- Iteration 22 --
int(0)
int(0)
-- Iteration 23 --
int(0)
int(0)
-- Iteration 24 --
int(0)
int(0)
-- Iteration 25 --
int(0)
int(0)
-- Iteration 26 --
int(0)
int(0)
-- Iteration 27 --
int(0)
int(0)
-- Iteration 28 --
int(0)
int(0)
-- Iteration 29 --
int(0)
int(0)
-- Iteration 30 --
int(0)
int(0)
-- Iteration 31 --
int(0)
int(0)
-- Iteration 32 --
int(0)
int(0)
-- Iteration 33 --
int(0)
int(0)
-- Iteration 34 --
int(0)
int(0)
-- Iteration 35 --
int(0)
int(0)
-- Iteration 36 --
int(0)
int(0)
-- Iteration 37 --
int(0)
int(0)
-- Iteration 38 --
int(0)
int(0)
-- Iteration 39 --
int(0)
int(0)
-- Iteration 40 --
int(0)
int(0)
-- Iteration 41 --
int(0)
int(0)
-- Iteration 42 --
int(0)
int(0)
-- Iteration 43 --
int(0)
int(0)
-- Iteration 44 --
int(0)
int(0)
-- Iteration 45 --
int(0)
int(0)
-- Iteration 46 --
int(0)
int(0)
-- Iteration 47 --
int(0)
int(0)
-- Iteration 48 --
int(0)
int(0)
-- Iteration 49 --
int(0)
int(0)
-- Iteration 50 --
int(0)
int(0)
-- Iteration 51 --
int(0)
int(0)
-- Iteration 52 --
int(0)
int(0)
-- Iteration 53 --
int(0)
int(0)
-- Iteration 54 --
int(0)
int(0)
-- Iteration 55 --
int(0)
int(0)
-- Iteration 56 --
int(0)
int(0)
-- Iteration 57 --
int(0)
int(0)
-- Iteration 58 --
int(0)
int(0)
-- Iteration 59 --
int(0)
int(0)
-- Iteration 60 --
int(0)
int(0)
-- Iteration 61 --
int(0)
int(0)
-- Iteration 62 --
int(0)
int(0)
-- Iteration 63 --
int(0)
int(0)
-- Iteration 64 --
int(0)
int(0)
-- Iteration 65 --
int(0)
int(0)
-- Iteration 66 --
int(0)
int(0)
-- Iteration 67 --
int(0)
int(0)
-- Iteration 68 --
int(0)
int(0)
-- Iteration 69 --
int(0)
int(0)
-- Iteration 70 --
int(0)
int(0)
-- Iteration 71 --
int(0)
int(0)
-- Iteration 72 --
int(0)
int(0)
-- Iteration 73 --
int(0)
int(0)
-- Iteration 74 --
int(0)
int(0)
-- Iteration 75 --
int(0)
int(0)
-- Iteration 76 --
int(0)
int(0)
-- Iteration 77 --
int(0)
int(0)
-- Iteration 78 --
int(0)
int(0)
-- Iteration 79 --
int(0)
int(0)
-- Iteration 80 --
int(0)
int(0)
-- Iteration 81 --
int(0)
int(0)
-- Iteration 82 --
int(0)
int(0)
-- Iteration 83 --
int(0)
int(0)
-- Iteration 84 --
int(0)
int(0)
-- Iteration 85 --
int(0)
int(0)
-- Iteration 86 --
int(0)
int(0)
-- Iteration 87 --
int(0)
int(0)
-- Iteration 88 --
int(0)
int(0)
-- Iteration 89 --
int(0)
int(0)
-- Iteration 90 --
int(0)
int(0)
-- Iteration 91 --
int(0)
int(0)
-- Iteration 92 --
int(0)
int(0)
-- Iteration 93 --
int(0)
int(0)
-- Iteration 94 --
int(0)
int(0)
-- Iteration 95 --
int(0)
int(0)
-- Iteration 96 --
int(0)
int(0)
-- Iteration 97 --
int(0)
int(0)
-- Iteration 98 --
int(0)
int(0)
-- Iteration 99 --
int(0)
int(0)
-- Iteration 100 --
int(0)
int(0)
-- Iteration 101 --
int(0)
int(0)
-- Iteration 102 --
int(0)
int(0)
-- Iteration 103 --
int(0)
int(0)
-- Iteration 104 --
int(0)
int(0)
-- Iteration 105 --
int(0)
int(0)
-- Iteration 106 --
int(0)
int(0)
-- Iteration 107 --
int(0)
int(0)
-- Iteration 108 --
int(0)
int(0)
-- Iteration 109 --
int(0)
int(0)
-- Iteration 110 --
int(0)
int(0)
-- Iteration 111 --
int(0)
int(0)
-- Iteration 112 --
int(0)
int(0)
-- Iteration 113 --
int(0)
int(0)
-- Iteration 114 --
int(0)
int(0)
-- Iteration 115 --
int(0)
int(0)
-- Iteration 116 --
int(0)
int(0)
-- Iteration 117 --
int(0)
int(0)
-- Iteration 118 --
int(0)
int(0)
-- Iteration 119 --
int(0)
int(0)
-- Iteration 120 --
int(0)
int(0)
-- Iteration 121 --
int(0)
int(0)
-- Iteration 122 --
int(0)
int(0)
-- Iteration 123 --
int(0)
int(0)
-- Iteration 124 --
int(0)
int(0)
-- Iteration 125 --
int(0)
int(0)
-- Iteration 126 --
int(0)
int(0)
-- Iteration 127 --
int(0)
int(0)
-- Iteration 128 --
int(0)
int(0)
-- Iteration 129 --
int(0)
int(0)
-- Iteration 130 --
int(0)
int(0)
-- Iteration 131 --
int(0)
int(0)
-- Iteration 132 --
int(0)
int(0)
-- Iteration 133 --
int(0)
int(0)
-- Iteration 134 --
int(0)
int(0)
-- Iteration 135 --
int(0)
int(0)
-- Iteration 136 --
int(0)
int(0)
-- Iteration 137 --
int(0)
int(0)
-- Iteration 138 --
int(0)
int(0)
-- Iteration 139 --
int(0)
int(0)
-- Iteration 140 --
int(0)
int(0)
-- Iteration 141 --
int(0)
int(0)
-- Iteration 142 --
int(0)
int(0)
-- Iteration 143 --
int(0)
int(0)
-- Iteration 144 --
int(0)
int(0)
-- Iteration 145 --
int(0)
int(0)
-- Iteration 146 --
int(0)
int(0)
-- Iteration 147 --
int(0)
int(0)
-- Iteration 148 --
int(0)
int(0)
-- Iteration 149 --
int(0)
int(0)
-- Iteration 150 --
int(0)
int(0)
-- Iteration 151 --
int(0)
int(0)
-- Iteration 152 --
int(0)
int(0)
-- Iteration 153 --
int(0)
int(0)
-- Iteration 154 --
int(0)
int(0)
-- Iteration 155 --
int(0)
int(0)
-- Iteration 156 --
int(0)
int(0)
-- Iteration 157 --
int(0)
int(0)
-- Iteration 158 --
int(0)
int(0)
-- Iteration 159 --
int(0)
int(0)
-- Iteration 160 --
int(0)
int(0)
-- Iteration 161 --
int(0)
int(0)
-- Iteration 162 --
int(0)
int(0)
-- Iteration 163 --
int(0)
int(0)
-- Iteration 164 --
int(0)
int(0)
-- Iteration 165 --
int(0)
int(0)
-- Iteration 166 --
int(0)
int(0)
-- Iteration 167 --
int(0)
int(0)
-- Iteration 168 --
int(0)
int(0)
-- Iteration 169 --
int(0)
int(0)
-- Iteration 170 --
int(0)
int(0)
-- Iteration 171 --
int(0)
int(0)
-- Iteration 172 --
int(0)
int(0)
-- Iteration 173 --
int(0)
int(0)
-- Iteration 174 --
int(0)
int(0)
-- Iteration 175 --
int(0)
int(0)
-- Iteration 176 --
int(0)
int(0)
-- Iteration 177 --
int(0)
int(0)
-- Iteration 178 --
int(0)
int(0)
-- Iteration 179 --
int(0)
int(0)
-- Iteration 180 --
int(0)
int(0)
-- Iteration 181 --
int(0)
int(0)
-- Iteration 182 --
int(0)
int(0)
-- Iteration 183 --
int(0)
int(0)
-- Iteration 184 --
int(0)
int(0)
-- Iteration 185 --
int(0)
int(0)
-- Iteration 186 --
int(0)
int(0)
-- Iteration 187 --
int(0)
int(0)
-- Iteration 188 --
int(0)
int(0)
-- Iteration 189 --
int(0)
int(0)
-- Iteration 190 --
int(0)
int(0)
-- Iteration 191 --
int(0)
int(0)
-- Iteration 192 --
int(0)
int(0)
-- Iteration 193 --
int(0)
int(0)
-- Iteration 194 --
int(0)
int(0)
-- Iteration 195 --
int(0)
int(0)
-- Iteration 196 --
int(0)
int(0)
-- Iteration 197 --
int(0)
int(0)
-- Iteration 198 --
int(0)
int(0)
-- Iteration 199 --
int(0)
int(0)
-- Iteration 200 --
int(0)
int(0)
-- Iteration 201 --
int(0)
int(0)
-- Iteration 202 --
int(0)
int(0)
-- Iteration 203 --
int(0)
int(0)
-- Iteration 204 --
int(0)
int(0)
-- Iteration 205 --
int(0)
int(0)
-- Iteration 206 --
int(0)
int(0)
-- Iteration 207 --
int(0)
int(0)
-- Iteration 208 --
int(0)
int(0)
-- Iteration 209 --
int(0)
int(0)
-- Iteration 210 --
int(0)
int(0)
-- Iteration 211 --
int(0)
int(0)
-- Iteration 212 --
int(0)
int(0)
-- Iteration 213 --
int(0)
int(0)
-- Iteration 214 --
int(0)
int(0)
-- Iteration 215 --
int(0)
int(0)
-- Iteration 216 --
int(0)
int(0)
-- Iteration 217 --
int(0)
int(0)
-- Iteration 218 --
int(0)
int(0)
-- Iteration 219 --
int(0)
int(0)
-- Iteration 220 --
int(0)
int(0)
-- Iteration 221 --
int(0)
int(0)
-- Iteration 222 --
int(0)
int(0)
-- Iteration 223 --
int(0)
int(0)
-- Iteration 224 --
int(0)
int(0)
-- Iteration 225 --
int(0)
int(0)
-- Iteration 226 --
int(0)
int(0)
-- Iteration 227 --
int(0)
int(0)
-- Iteration 228 --
int(0)
int(0)
-- Iteration 229 --
int(0)
int(0)
-- Iteration 230 --
int(0)
int(0)
-- Iteration 231 --
int(0)
int(0)
-- Iteration 232 --
int(0)
int(0)
-- Iteration 233 --
int(0)
int(0)
-- Iteration 234 --
int(0)
int(0)
-- Iteration 235 --
int(0)
int(0)
-- Iteration 236 --
int(0)
int(0)
-- Iteration 237 --
int(0)
int(0)
-- Iteration 238 --
int(0)
int(0)
-- Iteration 239 --
int(0)
int(0)
-- Iteration 240 --
int(0)
int(0)
-- Iteration 241 --
int(0)
int(0)
-- Iteration 242 --
int(0)
int(0)
-- Iteration 243 --
int(0)
int(0)
-- Iteration 244 --
int(0)
int(0)
-- Iteration 245 --
int(0)
int(0)
-- Iteration 246 --
int(0)
int(0)
-- Iteration 247 --
int(0)
int(0)
-- Iteration 248 --
int(0)
int(0)
-- Iteration 249 --
int(0)
int(0)
-- Iteration 250 --
int(0)
int(0)
-- Iteration 251 --
int(0)
int(0)
-- Iteration 252 --
int(0)
int(0)
-- Iteration 253 --
int(0)
int(0)
-- Iteration 254 --
int(0)
int(0)
-- Iteration 255 --
int(0)
int(0)
-- Iteration 256 --
int(0)
int(0)

-- Checking with out of character's range, given in binary format --
int(1)

*** Done ***
