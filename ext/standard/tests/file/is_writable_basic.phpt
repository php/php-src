--TEST--
Test is_writable() and its alias is_writeable() function: basic functionality
--SKIPIF--
<?php
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
// include common file test functions
require __DIR__.'/file.inc';

echo "*** Testing is_writable(): basic functionality ***\n";

// create a file
$filename = __DIR__."/is_writable.tmp";
create_file($filename);

$counter = 1;
/* loop to check if the file with new mode is writable
   using is_writable() */
for($mode = 0000; $mode <= 0777; $mode++) {
  echo "-- Changing mode of file to $mode --\n";
  chmod($filename, $mode);  // change mode of file
  var_dump( is_writeable($filename) );
  var_dump( is_writable($filename) );
  clearstatcache();
  $counter++;
}

// delete the temp file
delete_file($filename);

echo "Done\n";
?>
--EXPECT--
*** Testing is_writable(): basic functionality ***
-- Changing mode of file to 0 --
bool(false)
bool(false)
-- Changing mode of file to 1 --
bool(false)
bool(false)
-- Changing mode of file to 2 --
bool(false)
bool(false)
-- Changing mode of file to 3 --
bool(false)
bool(false)
-- Changing mode of file to 4 --
bool(false)
bool(false)
-- Changing mode of file to 5 --
bool(false)
bool(false)
-- Changing mode of file to 6 --
bool(false)
bool(false)
-- Changing mode of file to 7 --
bool(false)
bool(false)
-- Changing mode of file to 8 --
bool(false)
bool(false)
-- Changing mode of file to 9 --
bool(false)
bool(false)
-- Changing mode of file to 10 --
bool(false)
bool(false)
-- Changing mode of file to 11 --
bool(false)
bool(false)
-- Changing mode of file to 12 --
bool(false)
bool(false)
-- Changing mode of file to 13 --
bool(false)
bool(false)
-- Changing mode of file to 14 --
bool(false)
bool(false)
-- Changing mode of file to 15 --
bool(false)
bool(false)
-- Changing mode of file to 16 --
bool(false)
bool(false)
-- Changing mode of file to 17 --
bool(false)
bool(false)
-- Changing mode of file to 18 --
bool(false)
bool(false)
-- Changing mode of file to 19 --
bool(false)
bool(false)
-- Changing mode of file to 20 --
bool(false)
bool(false)
-- Changing mode of file to 21 --
bool(false)
bool(false)
-- Changing mode of file to 22 --
bool(false)
bool(false)
-- Changing mode of file to 23 --
bool(false)
bool(false)
-- Changing mode of file to 24 --
bool(false)
bool(false)
-- Changing mode of file to 25 --
bool(false)
bool(false)
-- Changing mode of file to 26 --
bool(false)
bool(false)
-- Changing mode of file to 27 --
bool(false)
bool(false)
-- Changing mode of file to 28 --
bool(false)
bool(false)
-- Changing mode of file to 29 --
bool(false)
bool(false)
-- Changing mode of file to 30 --
bool(false)
bool(false)
-- Changing mode of file to 31 --
bool(false)
bool(false)
-- Changing mode of file to 32 --
bool(false)
bool(false)
-- Changing mode of file to 33 --
bool(false)
bool(false)
-- Changing mode of file to 34 --
bool(false)
bool(false)
-- Changing mode of file to 35 --
bool(false)
bool(false)
-- Changing mode of file to 36 --
bool(false)
bool(false)
-- Changing mode of file to 37 --
bool(false)
bool(false)
-- Changing mode of file to 38 --
bool(false)
bool(false)
-- Changing mode of file to 39 --
bool(false)
bool(false)
-- Changing mode of file to 40 --
bool(false)
bool(false)
-- Changing mode of file to 41 --
bool(false)
bool(false)
-- Changing mode of file to 42 --
bool(false)
bool(false)
-- Changing mode of file to 43 --
bool(false)
bool(false)
-- Changing mode of file to 44 --
bool(false)
bool(false)
-- Changing mode of file to 45 --
bool(false)
bool(false)
-- Changing mode of file to 46 --
bool(false)
bool(false)
-- Changing mode of file to 47 --
bool(false)
bool(false)
-- Changing mode of file to 48 --
bool(false)
bool(false)
-- Changing mode of file to 49 --
bool(false)
bool(false)
-- Changing mode of file to 50 --
bool(false)
bool(false)
-- Changing mode of file to 51 --
bool(false)
bool(false)
-- Changing mode of file to 52 --
bool(false)
bool(false)
-- Changing mode of file to 53 --
bool(false)
bool(false)
-- Changing mode of file to 54 --
bool(false)
bool(false)
-- Changing mode of file to 55 --
bool(false)
bool(false)
-- Changing mode of file to 56 --
bool(false)
bool(false)
-- Changing mode of file to 57 --
bool(false)
bool(false)
-- Changing mode of file to 58 --
bool(false)
bool(false)
-- Changing mode of file to 59 --
bool(false)
bool(false)
-- Changing mode of file to 60 --
bool(false)
bool(false)
-- Changing mode of file to 61 --
bool(false)
bool(false)
-- Changing mode of file to 62 --
bool(false)
bool(false)
-- Changing mode of file to 63 --
bool(false)
bool(false)
-- Changing mode of file to 64 --
bool(false)
bool(false)
-- Changing mode of file to 65 --
bool(false)
bool(false)
-- Changing mode of file to 66 --
bool(false)
bool(false)
-- Changing mode of file to 67 --
bool(false)
bool(false)
-- Changing mode of file to 68 --
bool(false)
bool(false)
-- Changing mode of file to 69 --
bool(false)
bool(false)
-- Changing mode of file to 70 --
bool(false)
bool(false)
-- Changing mode of file to 71 --
bool(false)
bool(false)
-- Changing mode of file to 72 --
bool(false)
bool(false)
-- Changing mode of file to 73 --
bool(false)
bool(false)
-- Changing mode of file to 74 --
bool(false)
bool(false)
-- Changing mode of file to 75 --
bool(false)
bool(false)
-- Changing mode of file to 76 --
bool(false)
bool(false)
-- Changing mode of file to 77 --
bool(false)
bool(false)
-- Changing mode of file to 78 --
bool(false)
bool(false)
-- Changing mode of file to 79 --
bool(false)
bool(false)
-- Changing mode of file to 80 --
bool(false)
bool(false)
-- Changing mode of file to 81 --
bool(false)
bool(false)
-- Changing mode of file to 82 --
bool(false)
bool(false)
-- Changing mode of file to 83 --
bool(false)
bool(false)
-- Changing mode of file to 84 --
bool(false)
bool(false)
-- Changing mode of file to 85 --
bool(false)
bool(false)
-- Changing mode of file to 86 --
bool(false)
bool(false)
-- Changing mode of file to 87 --
bool(false)
bool(false)
-- Changing mode of file to 88 --
bool(false)
bool(false)
-- Changing mode of file to 89 --
bool(false)
bool(false)
-- Changing mode of file to 90 --
bool(false)
bool(false)
-- Changing mode of file to 91 --
bool(false)
bool(false)
-- Changing mode of file to 92 --
bool(false)
bool(false)
-- Changing mode of file to 93 --
bool(false)
bool(false)
-- Changing mode of file to 94 --
bool(false)
bool(false)
-- Changing mode of file to 95 --
bool(false)
bool(false)
-- Changing mode of file to 96 --
bool(false)
bool(false)
-- Changing mode of file to 97 --
bool(false)
bool(false)
-- Changing mode of file to 98 --
bool(false)
bool(false)
-- Changing mode of file to 99 --
bool(false)
bool(false)
-- Changing mode of file to 100 --
bool(false)
bool(false)
-- Changing mode of file to 101 --
bool(false)
bool(false)
-- Changing mode of file to 102 --
bool(false)
bool(false)
-- Changing mode of file to 103 --
bool(false)
bool(false)
-- Changing mode of file to 104 --
bool(false)
bool(false)
-- Changing mode of file to 105 --
bool(false)
bool(false)
-- Changing mode of file to 106 --
bool(false)
bool(false)
-- Changing mode of file to 107 --
bool(false)
bool(false)
-- Changing mode of file to 108 --
bool(false)
bool(false)
-- Changing mode of file to 109 --
bool(false)
bool(false)
-- Changing mode of file to 110 --
bool(false)
bool(false)
-- Changing mode of file to 111 --
bool(false)
bool(false)
-- Changing mode of file to 112 --
bool(false)
bool(false)
-- Changing mode of file to 113 --
bool(false)
bool(false)
-- Changing mode of file to 114 --
bool(false)
bool(false)
-- Changing mode of file to 115 --
bool(false)
bool(false)
-- Changing mode of file to 116 --
bool(false)
bool(false)
-- Changing mode of file to 117 --
bool(false)
bool(false)
-- Changing mode of file to 118 --
bool(false)
bool(false)
-- Changing mode of file to 119 --
bool(false)
bool(false)
-- Changing mode of file to 120 --
bool(false)
bool(false)
-- Changing mode of file to 121 --
bool(false)
bool(false)
-- Changing mode of file to 122 --
bool(false)
bool(false)
-- Changing mode of file to 123 --
bool(false)
bool(false)
-- Changing mode of file to 124 --
bool(false)
bool(false)
-- Changing mode of file to 125 --
bool(false)
bool(false)
-- Changing mode of file to 126 --
bool(false)
bool(false)
-- Changing mode of file to 127 --
bool(false)
bool(false)
-- Changing mode of file to 128 --
bool(true)
bool(true)
-- Changing mode of file to 129 --
bool(true)
bool(true)
-- Changing mode of file to 130 --
bool(true)
bool(true)
-- Changing mode of file to 131 --
bool(true)
bool(true)
-- Changing mode of file to 132 --
bool(true)
bool(true)
-- Changing mode of file to 133 --
bool(true)
bool(true)
-- Changing mode of file to 134 --
bool(true)
bool(true)
-- Changing mode of file to 135 --
bool(true)
bool(true)
-- Changing mode of file to 136 --
bool(true)
bool(true)
-- Changing mode of file to 137 --
bool(true)
bool(true)
-- Changing mode of file to 138 --
bool(true)
bool(true)
-- Changing mode of file to 139 --
bool(true)
bool(true)
-- Changing mode of file to 140 --
bool(true)
bool(true)
-- Changing mode of file to 141 --
bool(true)
bool(true)
-- Changing mode of file to 142 --
bool(true)
bool(true)
-- Changing mode of file to 143 --
bool(true)
bool(true)
-- Changing mode of file to 144 --
bool(true)
bool(true)
-- Changing mode of file to 145 --
bool(true)
bool(true)
-- Changing mode of file to 146 --
bool(true)
bool(true)
-- Changing mode of file to 147 --
bool(true)
bool(true)
-- Changing mode of file to 148 --
bool(true)
bool(true)
-- Changing mode of file to 149 --
bool(true)
bool(true)
-- Changing mode of file to 150 --
bool(true)
bool(true)
-- Changing mode of file to 151 --
bool(true)
bool(true)
-- Changing mode of file to 152 --
bool(true)
bool(true)
-- Changing mode of file to 153 --
bool(true)
bool(true)
-- Changing mode of file to 154 --
bool(true)
bool(true)
-- Changing mode of file to 155 --
bool(true)
bool(true)
-- Changing mode of file to 156 --
bool(true)
bool(true)
-- Changing mode of file to 157 --
bool(true)
bool(true)
-- Changing mode of file to 158 --
bool(true)
bool(true)
-- Changing mode of file to 159 --
bool(true)
bool(true)
-- Changing mode of file to 160 --
bool(true)
bool(true)
-- Changing mode of file to 161 --
bool(true)
bool(true)
-- Changing mode of file to 162 --
bool(true)
bool(true)
-- Changing mode of file to 163 --
bool(true)
bool(true)
-- Changing mode of file to 164 --
bool(true)
bool(true)
-- Changing mode of file to 165 --
bool(true)
bool(true)
-- Changing mode of file to 166 --
bool(true)
bool(true)
-- Changing mode of file to 167 --
bool(true)
bool(true)
-- Changing mode of file to 168 --
bool(true)
bool(true)
-- Changing mode of file to 169 --
bool(true)
bool(true)
-- Changing mode of file to 170 --
bool(true)
bool(true)
-- Changing mode of file to 171 --
bool(true)
bool(true)
-- Changing mode of file to 172 --
bool(true)
bool(true)
-- Changing mode of file to 173 --
bool(true)
bool(true)
-- Changing mode of file to 174 --
bool(true)
bool(true)
-- Changing mode of file to 175 --
bool(true)
bool(true)
-- Changing mode of file to 176 --
bool(true)
bool(true)
-- Changing mode of file to 177 --
bool(true)
bool(true)
-- Changing mode of file to 178 --
bool(true)
bool(true)
-- Changing mode of file to 179 --
bool(true)
bool(true)
-- Changing mode of file to 180 --
bool(true)
bool(true)
-- Changing mode of file to 181 --
bool(true)
bool(true)
-- Changing mode of file to 182 --
bool(true)
bool(true)
-- Changing mode of file to 183 --
bool(true)
bool(true)
-- Changing mode of file to 184 --
bool(true)
bool(true)
-- Changing mode of file to 185 --
bool(true)
bool(true)
-- Changing mode of file to 186 --
bool(true)
bool(true)
-- Changing mode of file to 187 --
bool(true)
bool(true)
-- Changing mode of file to 188 --
bool(true)
bool(true)
-- Changing mode of file to 189 --
bool(true)
bool(true)
-- Changing mode of file to 190 --
bool(true)
bool(true)
-- Changing mode of file to 191 --
bool(true)
bool(true)
-- Changing mode of file to 192 --
bool(true)
bool(true)
-- Changing mode of file to 193 --
bool(true)
bool(true)
-- Changing mode of file to 194 --
bool(true)
bool(true)
-- Changing mode of file to 195 --
bool(true)
bool(true)
-- Changing mode of file to 196 --
bool(true)
bool(true)
-- Changing mode of file to 197 --
bool(true)
bool(true)
-- Changing mode of file to 198 --
bool(true)
bool(true)
-- Changing mode of file to 199 --
bool(true)
bool(true)
-- Changing mode of file to 200 --
bool(true)
bool(true)
-- Changing mode of file to 201 --
bool(true)
bool(true)
-- Changing mode of file to 202 --
bool(true)
bool(true)
-- Changing mode of file to 203 --
bool(true)
bool(true)
-- Changing mode of file to 204 --
bool(true)
bool(true)
-- Changing mode of file to 205 --
bool(true)
bool(true)
-- Changing mode of file to 206 --
bool(true)
bool(true)
-- Changing mode of file to 207 --
bool(true)
bool(true)
-- Changing mode of file to 208 --
bool(true)
bool(true)
-- Changing mode of file to 209 --
bool(true)
bool(true)
-- Changing mode of file to 210 --
bool(true)
bool(true)
-- Changing mode of file to 211 --
bool(true)
bool(true)
-- Changing mode of file to 212 --
bool(true)
bool(true)
-- Changing mode of file to 213 --
bool(true)
bool(true)
-- Changing mode of file to 214 --
bool(true)
bool(true)
-- Changing mode of file to 215 --
bool(true)
bool(true)
-- Changing mode of file to 216 --
bool(true)
bool(true)
-- Changing mode of file to 217 --
bool(true)
bool(true)
-- Changing mode of file to 218 --
bool(true)
bool(true)
-- Changing mode of file to 219 --
bool(true)
bool(true)
-- Changing mode of file to 220 --
bool(true)
bool(true)
-- Changing mode of file to 221 --
bool(true)
bool(true)
-- Changing mode of file to 222 --
bool(true)
bool(true)
-- Changing mode of file to 223 --
bool(true)
bool(true)
-- Changing mode of file to 224 --
bool(true)
bool(true)
-- Changing mode of file to 225 --
bool(true)
bool(true)
-- Changing mode of file to 226 --
bool(true)
bool(true)
-- Changing mode of file to 227 --
bool(true)
bool(true)
-- Changing mode of file to 228 --
bool(true)
bool(true)
-- Changing mode of file to 229 --
bool(true)
bool(true)
-- Changing mode of file to 230 --
bool(true)
bool(true)
-- Changing mode of file to 231 --
bool(true)
bool(true)
-- Changing mode of file to 232 --
bool(true)
bool(true)
-- Changing mode of file to 233 --
bool(true)
bool(true)
-- Changing mode of file to 234 --
bool(true)
bool(true)
-- Changing mode of file to 235 --
bool(true)
bool(true)
-- Changing mode of file to 236 --
bool(true)
bool(true)
-- Changing mode of file to 237 --
bool(true)
bool(true)
-- Changing mode of file to 238 --
bool(true)
bool(true)
-- Changing mode of file to 239 --
bool(true)
bool(true)
-- Changing mode of file to 240 --
bool(true)
bool(true)
-- Changing mode of file to 241 --
bool(true)
bool(true)
-- Changing mode of file to 242 --
bool(true)
bool(true)
-- Changing mode of file to 243 --
bool(true)
bool(true)
-- Changing mode of file to 244 --
bool(true)
bool(true)
-- Changing mode of file to 245 --
bool(true)
bool(true)
-- Changing mode of file to 246 --
bool(true)
bool(true)
-- Changing mode of file to 247 --
bool(true)
bool(true)
-- Changing mode of file to 248 --
bool(true)
bool(true)
-- Changing mode of file to 249 --
bool(true)
bool(true)
-- Changing mode of file to 250 --
bool(true)
bool(true)
-- Changing mode of file to 251 --
bool(true)
bool(true)
-- Changing mode of file to 252 --
bool(true)
bool(true)
-- Changing mode of file to 253 --
bool(true)
bool(true)
-- Changing mode of file to 254 --
bool(true)
bool(true)
-- Changing mode of file to 255 --
bool(true)
bool(true)
-- Changing mode of file to 256 --
bool(false)
bool(false)
-- Changing mode of file to 257 --
bool(false)
bool(false)
-- Changing mode of file to 258 --
bool(false)
bool(false)
-- Changing mode of file to 259 --
bool(false)
bool(false)
-- Changing mode of file to 260 --
bool(false)
bool(false)
-- Changing mode of file to 261 --
bool(false)
bool(false)
-- Changing mode of file to 262 --
bool(false)
bool(false)
-- Changing mode of file to 263 --
bool(false)
bool(false)
-- Changing mode of file to 264 --
bool(false)
bool(false)
-- Changing mode of file to 265 --
bool(false)
bool(false)
-- Changing mode of file to 266 --
bool(false)
bool(false)
-- Changing mode of file to 267 --
bool(false)
bool(false)
-- Changing mode of file to 268 --
bool(false)
bool(false)
-- Changing mode of file to 269 --
bool(false)
bool(false)
-- Changing mode of file to 270 --
bool(false)
bool(false)
-- Changing mode of file to 271 --
bool(false)
bool(false)
-- Changing mode of file to 272 --
bool(false)
bool(false)
-- Changing mode of file to 273 --
bool(false)
bool(false)
-- Changing mode of file to 274 --
bool(false)
bool(false)
-- Changing mode of file to 275 --
bool(false)
bool(false)
-- Changing mode of file to 276 --
bool(false)
bool(false)
-- Changing mode of file to 277 --
bool(false)
bool(false)
-- Changing mode of file to 278 --
bool(false)
bool(false)
-- Changing mode of file to 279 --
bool(false)
bool(false)
-- Changing mode of file to 280 --
bool(false)
bool(false)
-- Changing mode of file to 281 --
bool(false)
bool(false)
-- Changing mode of file to 282 --
bool(false)
bool(false)
-- Changing mode of file to 283 --
bool(false)
bool(false)
-- Changing mode of file to 284 --
bool(false)
bool(false)
-- Changing mode of file to 285 --
bool(false)
bool(false)
-- Changing mode of file to 286 --
bool(false)
bool(false)
-- Changing mode of file to 287 --
bool(false)
bool(false)
-- Changing mode of file to 288 --
bool(false)
bool(false)
-- Changing mode of file to 289 --
bool(false)
bool(false)
-- Changing mode of file to 290 --
bool(false)
bool(false)
-- Changing mode of file to 291 --
bool(false)
bool(false)
-- Changing mode of file to 292 --
bool(false)
bool(false)
-- Changing mode of file to 293 --
bool(false)
bool(false)
-- Changing mode of file to 294 --
bool(false)
bool(false)
-- Changing mode of file to 295 --
bool(false)
bool(false)
-- Changing mode of file to 296 --
bool(false)
bool(false)
-- Changing mode of file to 297 --
bool(false)
bool(false)
-- Changing mode of file to 298 --
bool(false)
bool(false)
-- Changing mode of file to 299 --
bool(false)
bool(false)
-- Changing mode of file to 300 --
bool(false)
bool(false)
-- Changing mode of file to 301 --
bool(false)
bool(false)
-- Changing mode of file to 302 --
bool(false)
bool(false)
-- Changing mode of file to 303 --
bool(false)
bool(false)
-- Changing mode of file to 304 --
bool(false)
bool(false)
-- Changing mode of file to 305 --
bool(false)
bool(false)
-- Changing mode of file to 306 --
bool(false)
bool(false)
-- Changing mode of file to 307 --
bool(false)
bool(false)
-- Changing mode of file to 308 --
bool(false)
bool(false)
-- Changing mode of file to 309 --
bool(false)
bool(false)
-- Changing mode of file to 310 --
bool(false)
bool(false)
-- Changing mode of file to 311 --
bool(false)
bool(false)
-- Changing mode of file to 312 --
bool(false)
bool(false)
-- Changing mode of file to 313 --
bool(false)
bool(false)
-- Changing mode of file to 314 --
bool(false)
bool(false)
-- Changing mode of file to 315 --
bool(false)
bool(false)
-- Changing mode of file to 316 --
bool(false)
bool(false)
-- Changing mode of file to 317 --
bool(false)
bool(false)
-- Changing mode of file to 318 --
bool(false)
bool(false)
-- Changing mode of file to 319 --
bool(false)
bool(false)
-- Changing mode of file to 320 --
bool(false)
bool(false)
-- Changing mode of file to 321 --
bool(false)
bool(false)
-- Changing mode of file to 322 --
bool(false)
bool(false)
-- Changing mode of file to 323 --
bool(false)
bool(false)
-- Changing mode of file to 324 --
bool(false)
bool(false)
-- Changing mode of file to 325 --
bool(false)
bool(false)
-- Changing mode of file to 326 --
bool(false)
bool(false)
-- Changing mode of file to 327 --
bool(false)
bool(false)
-- Changing mode of file to 328 --
bool(false)
bool(false)
-- Changing mode of file to 329 --
bool(false)
bool(false)
-- Changing mode of file to 330 --
bool(false)
bool(false)
-- Changing mode of file to 331 --
bool(false)
bool(false)
-- Changing mode of file to 332 --
bool(false)
bool(false)
-- Changing mode of file to 333 --
bool(false)
bool(false)
-- Changing mode of file to 334 --
bool(false)
bool(false)
-- Changing mode of file to 335 --
bool(false)
bool(false)
-- Changing mode of file to 336 --
bool(false)
bool(false)
-- Changing mode of file to 337 --
bool(false)
bool(false)
-- Changing mode of file to 338 --
bool(false)
bool(false)
-- Changing mode of file to 339 --
bool(false)
bool(false)
-- Changing mode of file to 340 --
bool(false)
bool(false)
-- Changing mode of file to 341 --
bool(false)
bool(false)
-- Changing mode of file to 342 --
bool(false)
bool(false)
-- Changing mode of file to 343 --
bool(false)
bool(false)
-- Changing mode of file to 344 --
bool(false)
bool(false)
-- Changing mode of file to 345 --
bool(false)
bool(false)
-- Changing mode of file to 346 --
bool(false)
bool(false)
-- Changing mode of file to 347 --
bool(false)
bool(false)
-- Changing mode of file to 348 --
bool(false)
bool(false)
-- Changing mode of file to 349 --
bool(false)
bool(false)
-- Changing mode of file to 350 --
bool(false)
bool(false)
-- Changing mode of file to 351 --
bool(false)
bool(false)
-- Changing mode of file to 352 --
bool(false)
bool(false)
-- Changing mode of file to 353 --
bool(false)
bool(false)
-- Changing mode of file to 354 --
bool(false)
bool(false)
-- Changing mode of file to 355 --
bool(false)
bool(false)
-- Changing mode of file to 356 --
bool(false)
bool(false)
-- Changing mode of file to 357 --
bool(false)
bool(false)
-- Changing mode of file to 358 --
bool(false)
bool(false)
-- Changing mode of file to 359 --
bool(false)
bool(false)
-- Changing mode of file to 360 --
bool(false)
bool(false)
-- Changing mode of file to 361 --
bool(false)
bool(false)
-- Changing mode of file to 362 --
bool(false)
bool(false)
-- Changing mode of file to 363 --
bool(false)
bool(false)
-- Changing mode of file to 364 --
bool(false)
bool(false)
-- Changing mode of file to 365 --
bool(false)
bool(false)
-- Changing mode of file to 366 --
bool(false)
bool(false)
-- Changing mode of file to 367 --
bool(false)
bool(false)
-- Changing mode of file to 368 --
bool(false)
bool(false)
-- Changing mode of file to 369 --
bool(false)
bool(false)
-- Changing mode of file to 370 --
bool(false)
bool(false)
-- Changing mode of file to 371 --
bool(false)
bool(false)
-- Changing mode of file to 372 --
bool(false)
bool(false)
-- Changing mode of file to 373 --
bool(false)
bool(false)
-- Changing mode of file to 374 --
bool(false)
bool(false)
-- Changing mode of file to 375 --
bool(false)
bool(false)
-- Changing mode of file to 376 --
bool(false)
bool(false)
-- Changing mode of file to 377 --
bool(false)
bool(false)
-- Changing mode of file to 378 --
bool(false)
bool(false)
-- Changing mode of file to 379 --
bool(false)
bool(false)
-- Changing mode of file to 380 --
bool(false)
bool(false)
-- Changing mode of file to 381 --
bool(false)
bool(false)
-- Changing mode of file to 382 --
bool(false)
bool(false)
-- Changing mode of file to 383 --
bool(false)
bool(false)
-- Changing mode of file to 384 --
bool(true)
bool(true)
-- Changing mode of file to 385 --
bool(true)
bool(true)
-- Changing mode of file to 386 --
bool(true)
bool(true)
-- Changing mode of file to 387 --
bool(true)
bool(true)
-- Changing mode of file to 388 --
bool(true)
bool(true)
-- Changing mode of file to 389 --
bool(true)
bool(true)
-- Changing mode of file to 390 --
bool(true)
bool(true)
-- Changing mode of file to 391 --
bool(true)
bool(true)
-- Changing mode of file to 392 --
bool(true)
bool(true)
-- Changing mode of file to 393 --
bool(true)
bool(true)
-- Changing mode of file to 394 --
bool(true)
bool(true)
-- Changing mode of file to 395 --
bool(true)
bool(true)
-- Changing mode of file to 396 --
bool(true)
bool(true)
-- Changing mode of file to 397 --
bool(true)
bool(true)
-- Changing mode of file to 398 --
bool(true)
bool(true)
-- Changing mode of file to 399 --
bool(true)
bool(true)
-- Changing mode of file to 400 --
bool(true)
bool(true)
-- Changing mode of file to 401 --
bool(true)
bool(true)
-- Changing mode of file to 402 --
bool(true)
bool(true)
-- Changing mode of file to 403 --
bool(true)
bool(true)
-- Changing mode of file to 404 --
bool(true)
bool(true)
-- Changing mode of file to 405 --
bool(true)
bool(true)
-- Changing mode of file to 406 --
bool(true)
bool(true)
-- Changing mode of file to 407 --
bool(true)
bool(true)
-- Changing mode of file to 408 --
bool(true)
bool(true)
-- Changing mode of file to 409 --
bool(true)
bool(true)
-- Changing mode of file to 410 --
bool(true)
bool(true)
-- Changing mode of file to 411 --
bool(true)
bool(true)
-- Changing mode of file to 412 --
bool(true)
bool(true)
-- Changing mode of file to 413 --
bool(true)
bool(true)
-- Changing mode of file to 414 --
bool(true)
bool(true)
-- Changing mode of file to 415 --
bool(true)
bool(true)
-- Changing mode of file to 416 --
bool(true)
bool(true)
-- Changing mode of file to 417 --
bool(true)
bool(true)
-- Changing mode of file to 418 --
bool(true)
bool(true)
-- Changing mode of file to 419 --
bool(true)
bool(true)
-- Changing mode of file to 420 --
bool(true)
bool(true)
-- Changing mode of file to 421 --
bool(true)
bool(true)
-- Changing mode of file to 422 --
bool(true)
bool(true)
-- Changing mode of file to 423 --
bool(true)
bool(true)
-- Changing mode of file to 424 --
bool(true)
bool(true)
-- Changing mode of file to 425 --
bool(true)
bool(true)
-- Changing mode of file to 426 --
bool(true)
bool(true)
-- Changing mode of file to 427 --
bool(true)
bool(true)
-- Changing mode of file to 428 --
bool(true)
bool(true)
-- Changing mode of file to 429 --
bool(true)
bool(true)
-- Changing mode of file to 430 --
bool(true)
bool(true)
-- Changing mode of file to 431 --
bool(true)
bool(true)
-- Changing mode of file to 432 --
bool(true)
bool(true)
-- Changing mode of file to 433 --
bool(true)
bool(true)
-- Changing mode of file to 434 --
bool(true)
bool(true)
-- Changing mode of file to 435 --
bool(true)
bool(true)
-- Changing mode of file to 436 --
bool(true)
bool(true)
-- Changing mode of file to 437 --
bool(true)
bool(true)
-- Changing mode of file to 438 --
bool(true)
bool(true)
-- Changing mode of file to 439 --
bool(true)
bool(true)
-- Changing mode of file to 440 --
bool(true)
bool(true)
-- Changing mode of file to 441 --
bool(true)
bool(true)
-- Changing mode of file to 442 --
bool(true)
bool(true)
-- Changing mode of file to 443 --
bool(true)
bool(true)
-- Changing mode of file to 444 --
bool(true)
bool(true)
-- Changing mode of file to 445 --
bool(true)
bool(true)
-- Changing mode of file to 446 --
bool(true)
bool(true)
-- Changing mode of file to 447 --
bool(true)
bool(true)
-- Changing mode of file to 448 --
bool(true)
bool(true)
-- Changing mode of file to 449 --
bool(true)
bool(true)
-- Changing mode of file to 450 --
bool(true)
bool(true)
-- Changing mode of file to 451 --
bool(true)
bool(true)
-- Changing mode of file to 452 --
bool(true)
bool(true)
-- Changing mode of file to 453 --
bool(true)
bool(true)
-- Changing mode of file to 454 --
bool(true)
bool(true)
-- Changing mode of file to 455 --
bool(true)
bool(true)
-- Changing mode of file to 456 --
bool(true)
bool(true)
-- Changing mode of file to 457 --
bool(true)
bool(true)
-- Changing mode of file to 458 --
bool(true)
bool(true)
-- Changing mode of file to 459 --
bool(true)
bool(true)
-- Changing mode of file to 460 --
bool(true)
bool(true)
-- Changing mode of file to 461 --
bool(true)
bool(true)
-- Changing mode of file to 462 --
bool(true)
bool(true)
-- Changing mode of file to 463 --
bool(true)
bool(true)
-- Changing mode of file to 464 --
bool(true)
bool(true)
-- Changing mode of file to 465 --
bool(true)
bool(true)
-- Changing mode of file to 466 --
bool(true)
bool(true)
-- Changing mode of file to 467 --
bool(true)
bool(true)
-- Changing mode of file to 468 --
bool(true)
bool(true)
-- Changing mode of file to 469 --
bool(true)
bool(true)
-- Changing mode of file to 470 --
bool(true)
bool(true)
-- Changing mode of file to 471 --
bool(true)
bool(true)
-- Changing mode of file to 472 --
bool(true)
bool(true)
-- Changing mode of file to 473 --
bool(true)
bool(true)
-- Changing mode of file to 474 --
bool(true)
bool(true)
-- Changing mode of file to 475 --
bool(true)
bool(true)
-- Changing mode of file to 476 --
bool(true)
bool(true)
-- Changing mode of file to 477 --
bool(true)
bool(true)
-- Changing mode of file to 478 --
bool(true)
bool(true)
-- Changing mode of file to 479 --
bool(true)
bool(true)
-- Changing mode of file to 480 --
bool(true)
bool(true)
-- Changing mode of file to 481 --
bool(true)
bool(true)
-- Changing mode of file to 482 --
bool(true)
bool(true)
-- Changing mode of file to 483 --
bool(true)
bool(true)
-- Changing mode of file to 484 --
bool(true)
bool(true)
-- Changing mode of file to 485 --
bool(true)
bool(true)
-- Changing mode of file to 486 --
bool(true)
bool(true)
-- Changing mode of file to 487 --
bool(true)
bool(true)
-- Changing mode of file to 488 --
bool(true)
bool(true)
-- Changing mode of file to 489 --
bool(true)
bool(true)
-- Changing mode of file to 490 --
bool(true)
bool(true)
-- Changing mode of file to 491 --
bool(true)
bool(true)
-- Changing mode of file to 492 --
bool(true)
bool(true)
-- Changing mode of file to 493 --
bool(true)
bool(true)
-- Changing mode of file to 494 --
bool(true)
bool(true)
-- Changing mode of file to 495 --
bool(true)
bool(true)
-- Changing mode of file to 496 --
bool(true)
bool(true)
-- Changing mode of file to 497 --
bool(true)
bool(true)
-- Changing mode of file to 498 --
bool(true)
bool(true)
-- Changing mode of file to 499 --
bool(true)
bool(true)
-- Changing mode of file to 500 --
bool(true)
bool(true)
-- Changing mode of file to 501 --
bool(true)
bool(true)
-- Changing mode of file to 502 --
bool(true)
bool(true)
-- Changing mode of file to 503 --
bool(true)
bool(true)
-- Changing mode of file to 504 --
bool(true)
bool(true)
-- Changing mode of file to 505 --
bool(true)
bool(true)
-- Changing mode of file to 506 --
bool(true)
bool(true)
-- Changing mode of file to 507 --
bool(true)
bool(true)
-- Changing mode of file to 508 --
bool(true)
bool(true)
-- Changing mode of file to 509 --
bool(true)
bool(true)
-- Changing mode of file to 510 --
bool(true)
bool(true)
-- Changing mode of file to 511 --
bool(true)
bool(true)
Done
