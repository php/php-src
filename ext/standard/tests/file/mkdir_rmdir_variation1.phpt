--TEST--
Test mkdir() and rmdir() functions: usage variations - perms(0000-0777)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only on LINUX');
}
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
/*  Prototype: bool mkdir ( string $pathname [, int $mode [, bool $recursive [, resource $context]]] );
    Description: Makes directory
*/

echo "*** Testing mkdir() and rmdir() for different permissions ***\n";

$context = stream_context_create();

$file_path = __DIR__;
$counter = 1;

for($mode = 0000; $mode <= 0777; $mode++) {
  echo "-- Changing mode of directory to $mode --\n";
  var_dump( mkdir("$file_path/mkdir_variation1/", $mode, true) );
  var_dump( rmdir("$file_path/mkdir_variation1/") );
  $counter++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing mkdir() and rmdir() for different permissions ***
-- Changing mode of directory to 0 --
bool(true)
bool(true)
-- Changing mode of directory to 1 --
bool(true)
bool(true)
-- Changing mode of directory to 2 --
bool(true)
bool(true)
-- Changing mode of directory to 3 --
bool(true)
bool(true)
-- Changing mode of directory to 4 --
bool(true)
bool(true)
-- Changing mode of directory to 5 --
bool(true)
bool(true)
-- Changing mode of directory to 6 --
bool(true)
bool(true)
-- Changing mode of directory to 7 --
bool(true)
bool(true)
-- Changing mode of directory to 8 --
bool(true)
bool(true)
-- Changing mode of directory to 9 --
bool(true)
bool(true)
-- Changing mode of directory to 10 --
bool(true)
bool(true)
-- Changing mode of directory to 11 --
bool(true)
bool(true)
-- Changing mode of directory to 12 --
bool(true)
bool(true)
-- Changing mode of directory to 13 --
bool(true)
bool(true)
-- Changing mode of directory to 14 --
bool(true)
bool(true)
-- Changing mode of directory to 15 --
bool(true)
bool(true)
-- Changing mode of directory to 16 --
bool(true)
bool(true)
-- Changing mode of directory to 17 --
bool(true)
bool(true)
-- Changing mode of directory to 18 --
bool(true)
bool(true)
-- Changing mode of directory to 19 --
bool(true)
bool(true)
-- Changing mode of directory to 20 --
bool(true)
bool(true)
-- Changing mode of directory to 21 --
bool(true)
bool(true)
-- Changing mode of directory to 22 --
bool(true)
bool(true)
-- Changing mode of directory to 23 --
bool(true)
bool(true)
-- Changing mode of directory to 24 --
bool(true)
bool(true)
-- Changing mode of directory to 25 --
bool(true)
bool(true)
-- Changing mode of directory to 26 --
bool(true)
bool(true)
-- Changing mode of directory to 27 --
bool(true)
bool(true)
-- Changing mode of directory to 28 --
bool(true)
bool(true)
-- Changing mode of directory to 29 --
bool(true)
bool(true)
-- Changing mode of directory to 30 --
bool(true)
bool(true)
-- Changing mode of directory to 31 --
bool(true)
bool(true)
-- Changing mode of directory to 32 --
bool(true)
bool(true)
-- Changing mode of directory to 33 --
bool(true)
bool(true)
-- Changing mode of directory to 34 --
bool(true)
bool(true)
-- Changing mode of directory to 35 --
bool(true)
bool(true)
-- Changing mode of directory to 36 --
bool(true)
bool(true)
-- Changing mode of directory to 37 --
bool(true)
bool(true)
-- Changing mode of directory to 38 --
bool(true)
bool(true)
-- Changing mode of directory to 39 --
bool(true)
bool(true)
-- Changing mode of directory to 40 --
bool(true)
bool(true)
-- Changing mode of directory to 41 --
bool(true)
bool(true)
-- Changing mode of directory to 42 --
bool(true)
bool(true)
-- Changing mode of directory to 43 --
bool(true)
bool(true)
-- Changing mode of directory to 44 --
bool(true)
bool(true)
-- Changing mode of directory to 45 --
bool(true)
bool(true)
-- Changing mode of directory to 46 --
bool(true)
bool(true)
-- Changing mode of directory to 47 --
bool(true)
bool(true)
-- Changing mode of directory to 48 --
bool(true)
bool(true)
-- Changing mode of directory to 49 --
bool(true)
bool(true)
-- Changing mode of directory to 50 --
bool(true)
bool(true)
-- Changing mode of directory to 51 --
bool(true)
bool(true)
-- Changing mode of directory to 52 --
bool(true)
bool(true)
-- Changing mode of directory to 53 --
bool(true)
bool(true)
-- Changing mode of directory to 54 --
bool(true)
bool(true)
-- Changing mode of directory to 55 --
bool(true)
bool(true)
-- Changing mode of directory to 56 --
bool(true)
bool(true)
-- Changing mode of directory to 57 --
bool(true)
bool(true)
-- Changing mode of directory to 58 --
bool(true)
bool(true)
-- Changing mode of directory to 59 --
bool(true)
bool(true)
-- Changing mode of directory to 60 --
bool(true)
bool(true)
-- Changing mode of directory to 61 --
bool(true)
bool(true)
-- Changing mode of directory to 62 --
bool(true)
bool(true)
-- Changing mode of directory to 63 --
bool(true)
bool(true)
-- Changing mode of directory to 64 --
bool(true)
bool(true)
-- Changing mode of directory to 65 --
bool(true)
bool(true)
-- Changing mode of directory to 66 --
bool(true)
bool(true)
-- Changing mode of directory to 67 --
bool(true)
bool(true)
-- Changing mode of directory to 68 --
bool(true)
bool(true)
-- Changing mode of directory to 69 --
bool(true)
bool(true)
-- Changing mode of directory to 70 --
bool(true)
bool(true)
-- Changing mode of directory to 71 --
bool(true)
bool(true)
-- Changing mode of directory to 72 --
bool(true)
bool(true)
-- Changing mode of directory to 73 --
bool(true)
bool(true)
-- Changing mode of directory to 74 --
bool(true)
bool(true)
-- Changing mode of directory to 75 --
bool(true)
bool(true)
-- Changing mode of directory to 76 --
bool(true)
bool(true)
-- Changing mode of directory to 77 --
bool(true)
bool(true)
-- Changing mode of directory to 78 --
bool(true)
bool(true)
-- Changing mode of directory to 79 --
bool(true)
bool(true)
-- Changing mode of directory to 80 --
bool(true)
bool(true)
-- Changing mode of directory to 81 --
bool(true)
bool(true)
-- Changing mode of directory to 82 --
bool(true)
bool(true)
-- Changing mode of directory to 83 --
bool(true)
bool(true)
-- Changing mode of directory to 84 --
bool(true)
bool(true)
-- Changing mode of directory to 85 --
bool(true)
bool(true)
-- Changing mode of directory to 86 --
bool(true)
bool(true)
-- Changing mode of directory to 87 --
bool(true)
bool(true)
-- Changing mode of directory to 88 --
bool(true)
bool(true)
-- Changing mode of directory to 89 --
bool(true)
bool(true)
-- Changing mode of directory to 90 --
bool(true)
bool(true)
-- Changing mode of directory to 91 --
bool(true)
bool(true)
-- Changing mode of directory to 92 --
bool(true)
bool(true)
-- Changing mode of directory to 93 --
bool(true)
bool(true)
-- Changing mode of directory to 94 --
bool(true)
bool(true)
-- Changing mode of directory to 95 --
bool(true)
bool(true)
-- Changing mode of directory to 96 --
bool(true)
bool(true)
-- Changing mode of directory to 97 --
bool(true)
bool(true)
-- Changing mode of directory to 98 --
bool(true)
bool(true)
-- Changing mode of directory to 99 --
bool(true)
bool(true)
-- Changing mode of directory to 100 --
bool(true)
bool(true)
-- Changing mode of directory to 101 --
bool(true)
bool(true)
-- Changing mode of directory to 102 --
bool(true)
bool(true)
-- Changing mode of directory to 103 --
bool(true)
bool(true)
-- Changing mode of directory to 104 --
bool(true)
bool(true)
-- Changing mode of directory to 105 --
bool(true)
bool(true)
-- Changing mode of directory to 106 --
bool(true)
bool(true)
-- Changing mode of directory to 107 --
bool(true)
bool(true)
-- Changing mode of directory to 108 --
bool(true)
bool(true)
-- Changing mode of directory to 109 --
bool(true)
bool(true)
-- Changing mode of directory to 110 --
bool(true)
bool(true)
-- Changing mode of directory to 111 --
bool(true)
bool(true)
-- Changing mode of directory to 112 --
bool(true)
bool(true)
-- Changing mode of directory to 113 --
bool(true)
bool(true)
-- Changing mode of directory to 114 --
bool(true)
bool(true)
-- Changing mode of directory to 115 --
bool(true)
bool(true)
-- Changing mode of directory to 116 --
bool(true)
bool(true)
-- Changing mode of directory to 117 --
bool(true)
bool(true)
-- Changing mode of directory to 118 --
bool(true)
bool(true)
-- Changing mode of directory to 119 --
bool(true)
bool(true)
-- Changing mode of directory to 120 --
bool(true)
bool(true)
-- Changing mode of directory to 121 --
bool(true)
bool(true)
-- Changing mode of directory to 122 --
bool(true)
bool(true)
-- Changing mode of directory to 123 --
bool(true)
bool(true)
-- Changing mode of directory to 124 --
bool(true)
bool(true)
-- Changing mode of directory to 125 --
bool(true)
bool(true)
-- Changing mode of directory to 126 --
bool(true)
bool(true)
-- Changing mode of directory to 127 --
bool(true)
bool(true)
-- Changing mode of directory to 128 --
bool(true)
bool(true)
-- Changing mode of directory to 129 --
bool(true)
bool(true)
-- Changing mode of directory to 130 --
bool(true)
bool(true)
-- Changing mode of directory to 131 --
bool(true)
bool(true)
-- Changing mode of directory to 132 --
bool(true)
bool(true)
-- Changing mode of directory to 133 --
bool(true)
bool(true)
-- Changing mode of directory to 134 --
bool(true)
bool(true)
-- Changing mode of directory to 135 --
bool(true)
bool(true)
-- Changing mode of directory to 136 --
bool(true)
bool(true)
-- Changing mode of directory to 137 --
bool(true)
bool(true)
-- Changing mode of directory to 138 --
bool(true)
bool(true)
-- Changing mode of directory to 139 --
bool(true)
bool(true)
-- Changing mode of directory to 140 --
bool(true)
bool(true)
-- Changing mode of directory to 141 --
bool(true)
bool(true)
-- Changing mode of directory to 142 --
bool(true)
bool(true)
-- Changing mode of directory to 143 --
bool(true)
bool(true)
-- Changing mode of directory to 144 --
bool(true)
bool(true)
-- Changing mode of directory to 145 --
bool(true)
bool(true)
-- Changing mode of directory to 146 --
bool(true)
bool(true)
-- Changing mode of directory to 147 --
bool(true)
bool(true)
-- Changing mode of directory to 148 --
bool(true)
bool(true)
-- Changing mode of directory to 149 --
bool(true)
bool(true)
-- Changing mode of directory to 150 --
bool(true)
bool(true)
-- Changing mode of directory to 151 --
bool(true)
bool(true)
-- Changing mode of directory to 152 --
bool(true)
bool(true)
-- Changing mode of directory to 153 --
bool(true)
bool(true)
-- Changing mode of directory to 154 --
bool(true)
bool(true)
-- Changing mode of directory to 155 --
bool(true)
bool(true)
-- Changing mode of directory to 156 --
bool(true)
bool(true)
-- Changing mode of directory to 157 --
bool(true)
bool(true)
-- Changing mode of directory to 158 --
bool(true)
bool(true)
-- Changing mode of directory to 159 --
bool(true)
bool(true)
-- Changing mode of directory to 160 --
bool(true)
bool(true)
-- Changing mode of directory to 161 --
bool(true)
bool(true)
-- Changing mode of directory to 162 --
bool(true)
bool(true)
-- Changing mode of directory to 163 --
bool(true)
bool(true)
-- Changing mode of directory to 164 --
bool(true)
bool(true)
-- Changing mode of directory to 165 --
bool(true)
bool(true)
-- Changing mode of directory to 166 --
bool(true)
bool(true)
-- Changing mode of directory to 167 --
bool(true)
bool(true)
-- Changing mode of directory to 168 --
bool(true)
bool(true)
-- Changing mode of directory to 169 --
bool(true)
bool(true)
-- Changing mode of directory to 170 --
bool(true)
bool(true)
-- Changing mode of directory to 171 --
bool(true)
bool(true)
-- Changing mode of directory to 172 --
bool(true)
bool(true)
-- Changing mode of directory to 173 --
bool(true)
bool(true)
-- Changing mode of directory to 174 --
bool(true)
bool(true)
-- Changing mode of directory to 175 --
bool(true)
bool(true)
-- Changing mode of directory to 176 --
bool(true)
bool(true)
-- Changing mode of directory to 177 --
bool(true)
bool(true)
-- Changing mode of directory to 178 --
bool(true)
bool(true)
-- Changing mode of directory to 179 --
bool(true)
bool(true)
-- Changing mode of directory to 180 --
bool(true)
bool(true)
-- Changing mode of directory to 181 --
bool(true)
bool(true)
-- Changing mode of directory to 182 --
bool(true)
bool(true)
-- Changing mode of directory to 183 --
bool(true)
bool(true)
-- Changing mode of directory to 184 --
bool(true)
bool(true)
-- Changing mode of directory to 185 --
bool(true)
bool(true)
-- Changing mode of directory to 186 --
bool(true)
bool(true)
-- Changing mode of directory to 187 --
bool(true)
bool(true)
-- Changing mode of directory to 188 --
bool(true)
bool(true)
-- Changing mode of directory to 189 --
bool(true)
bool(true)
-- Changing mode of directory to 190 --
bool(true)
bool(true)
-- Changing mode of directory to 191 --
bool(true)
bool(true)
-- Changing mode of directory to 192 --
bool(true)
bool(true)
-- Changing mode of directory to 193 --
bool(true)
bool(true)
-- Changing mode of directory to 194 --
bool(true)
bool(true)
-- Changing mode of directory to 195 --
bool(true)
bool(true)
-- Changing mode of directory to 196 --
bool(true)
bool(true)
-- Changing mode of directory to 197 --
bool(true)
bool(true)
-- Changing mode of directory to 198 --
bool(true)
bool(true)
-- Changing mode of directory to 199 --
bool(true)
bool(true)
-- Changing mode of directory to 200 --
bool(true)
bool(true)
-- Changing mode of directory to 201 --
bool(true)
bool(true)
-- Changing mode of directory to 202 --
bool(true)
bool(true)
-- Changing mode of directory to 203 --
bool(true)
bool(true)
-- Changing mode of directory to 204 --
bool(true)
bool(true)
-- Changing mode of directory to 205 --
bool(true)
bool(true)
-- Changing mode of directory to 206 --
bool(true)
bool(true)
-- Changing mode of directory to 207 --
bool(true)
bool(true)
-- Changing mode of directory to 208 --
bool(true)
bool(true)
-- Changing mode of directory to 209 --
bool(true)
bool(true)
-- Changing mode of directory to 210 --
bool(true)
bool(true)
-- Changing mode of directory to 211 --
bool(true)
bool(true)
-- Changing mode of directory to 212 --
bool(true)
bool(true)
-- Changing mode of directory to 213 --
bool(true)
bool(true)
-- Changing mode of directory to 214 --
bool(true)
bool(true)
-- Changing mode of directory to 215 --
bool(true)
bool(true)
-- Changing mode of directory to 216 --
bool(true)
bool(true)
-- Changing mode of directory to 217 --
bool(true)
bool(true)
-- Changing mode of directory to 218 --
bool(true)
bool(true)
-- Changing mode of directory to 219 --
bool(true)
bool(true)
-- Changing mode of directory to 220 --
bool(true)
bool(true)
-- Changing mode of directory to 221 --
bool(true)
bool(true)
-- Changing mode of directory to 222 --
bool(true)
bool(true)
-- Changing mode of directory to 223 --
bool(true)
bool(true)
-- Changing mode of directory to 224 --
bool(true)
bool(true)
-- Changing mode of directory to 225 --
bool(true)
bool(true)
-- Changing mode of directory to 226 --
bool(true)
bool(true)
-- Changing mode of directory to 227 --
bool(true)
bool(true)
-- Changing mode of directory to 228 --
bool(true)
bool(true)
-- Changing mode of directory to 229 --
bool(true)
bool(true)
-- Changing mode of directory to 230 --
bool(true)
bool(true)
-- Changing mode of directory to 231 --
bool(true)
bool(true)
-- Changing mode of directory to 232 --
bool(true)
bool(true)
-- Changing mode of directory to 233 --
bool(true)
bool(true)
-- Changing mode of directory to 234 --
bool(true)
bool(true)
-- Changing mode of directory to 235 --
bool(true)
bool(true)
-- Changing mode of directory to 236 --
bool(true)
bool(true)
-- Changing mode of directory to 237 --
bool(true)
bool(true)
-- Changing mode of directory to 238 --
bool(true)
bool(true)
-- Changing mode of directory to 239 --
bool(true)
bool(true)
-- Changing mode of directory to 240 --
bool(true)
bool(true)
-- Changing mode of directory to 241 --
bool(true)
bool(true)
-- Changing mode of directory to 242 --
bool(true)
bool(true)
-- Changing mode of directory to 243 --
bool(true)
bool(true)
-- Changing mode of directory to 244 --
bool(true)
bool(true)
-- Changing mode of directory to 245 --
bool(true)
bool(true)
-- Changing mode of directory to 246 --
bool(true)
bool(true)
-- Changing mode of directory to 247 --
bool(true)
bool(true)
-- Changing mode of directory to 248 --
bool(true)
bool(true)
-- Changing mode of directory to 249 --
bool(true)
bool(true)
-- Changing mode of directory to 250 --
bool(true)
bool(true)
-- Changing mode of directory to 251 --
bool(true)
bool(true)
-- Changing mode of directory to 252 --
bool(true)
bool(true)
-- Changing mode of directory to 253 --
bool(true)
bool(true)
-- Changing mode of directory to 254 --
bool(true)
bool(true)
-- Changing mode of directory to 255 --
bool(true)
bool(true)
-- Changing mode of directory to 256 --
bool(true)
bool(true)
-- Changing mode of directory to 257 --
bool(true)
bool(true)
-- Changing mode of directory to 258 --
bool(true)
bool(true)
-- Changing mode of directory to 259 --
bool(true)
bool(true)
-- Changing mode of directory to 260 --
bool(true)
bool(true)
-- Changing mode of directory to 261 --
bool(true)
bool(true)
-- Changing mode of directory to 262 --
bool(true)
bool(true)
-- Changing mode of directory to 263 --
bool(true)
bool(true)
-- Changing mode of directory to 264 --
bool(true)
bool(true)
-- Changing mode of directory to 265 --
bool(true)
bool(true)
-- Changing mode of directory to 266 --
bool(true)
bool(true)
-- Changing mode of directory to 267 --
bool(true)
bool(true)
-- Changing mode of directory to 268 --
bool(true)
bool(true)
-- Changing mode of directory to 269 --
bool(true)
bool(true)
-- Changing mode of directory to 270 --
bool(true)
bool(true)
-- Changing mode of directory to 271 --
bool(true)
bool(true)
-- Changing mode of directory to 272 --
bool(true)
bool(true)
-- Changing mode of directory to 273 --
bool(true)
bool(true)
-- Changing mode of directory to 274 --
bool(true)
bool(true)
-- Changing mode of directory to 275 --
bool(true)
bool(true)
-- Changing mode of directory to 276 --
bool(true)
bool(true)
-- Changing mode of directory to 277 --
bool(true)
bool(true)
-- Changing mode of directory to 278 --
bool(true)
bool(true)
-- Changing mode of directory to 279 --
bool(true)
bool(true)
-- Changing mode of directory to 280 --
bool(true)
bool(true)
-- Changing mode of directory to 281 --
bool(true)
bool(true)
-- Changing mode of directory to 282 --
bool(true)
bool(true)
-- Changing mode of directory to 283 --
bool(true)
bool(true)
-- Changing mode of directory to 284 --
bool(true)
bool(true)
-- Changing mode of directory to 285 --
bool(true)
bool(true)
-- Changing mode of directory to 286 --
bool(true)
bool(true)
-- Changing mode of directory to 287 --
bool(true)
bool(true)
-- Changing mode of directory to 288 --
bool(true)
bool(true)
-- Changing mode of directory to 289 --
bool(true)
bool(true)
-- Changing mode of directory to 290 --
bool(true)
bool(true)
-- Changing mode of directory to 291 --
bool(true)
bool(true)
-- Changing mode of directory to 292 --
bool(true)
bool(true)
-- Changing mode of directory to 293 --
bool(true)
bool(true)
-- Changing mode of directory to 294 --
bool(true)
bool(true)
-- Changing mode of directory to 295 --
bool(true)
bool(true)
-- Changing mode of directory to 296 --
bool(true)
bool(true)
-- Changing mode of directory to 297 --
bool(true)
bool(true)
-- Changing mode of directory to 298 --
bool(true)
bool(true)
-- Changing mode of directory to 299 --
bool(true)
bool(true)
-- Changing mode of directory to 300 --
bool(true)
bool(true)
-- Changing mode of directory to 301 --
bool(true)
bool(true)
-- Changing mode of directory to 302 --
bool(true)
bool(true)
-- Changing mode of directory to 303 --
bool(true)
bool(true)
-- Changing mode of directory to 304 --
bool(true)
bool(true)
-- Changing mode of directory to 305 --
bool(true)
bool(true)
-- Changing mode of directory to 306 --
bool(true)
bool(true)
-- Changing mode of directory to 307 --
bool(true)
bool(true)
-- Changing mode of directory to 308 --
bool(true)
bool(true)
-- Changing mode of directory to 309 --
bool(true)
bool(true)
-- Changing mode of directory to 310 --
bool(true)
bool(true)
-- Changing mode of directory to 311 --
bool(true)
bool(true)
-- Changing mode of directory to 312 --
bool(true)
bool(true)
-- Changing mode of directory to 313 --
bool(true)
bool(true)
-- Changing mode of directory to 314 --
bool(true)
bool(true)
-- Changing mode of directory to 315 --
bool(true)
bool(true)
-- Changing mode of directory to 316 --
bool(true)
bool(true)
-- Changing mode of directory to 317 --
bool(true)
bool(true)
-- Changing mode of directory to 318 --
bool(true)
bool(true)
-- Changing mode of directory to 319 --
bool(true)
bool(true)
-- Changing mode of directory to 320 --
bool(true)
bool(true)
-- Changing mode of directory to 321 --
bool(true)
bool(true)
-- Changing mode of directory to 322 --
bool(true)
bool(true)
-- Changing mode of directory to 323 --
bool(true)
bool(true)
-- Changing mode of directory to 324 --
bool(true)
bool(true)
-- Changing mode of directory to 325 --
bool(true)
bool(true)
-- Changing mode of directory to 326 --
bool(true)
bool(true)
-- Changing mode of directory to 327 --
bool(true)
bool(true)
-- Changing mode of directory to 328 --
bool(true)
bool(true)
-- Changing mode of directory to 329 --
bool(true)
bool(true)
-- Changing mode of directory to 330 --
bool(true)
bool(true)
-- Changing mode of directory to 331 --
bool(true)
bool(true)
-- Changing mode of directory to 332 --
bool(true)
bool(true)
-- Changing mode of directory to 333 --
bool(true)
bool(true)
-- Changing mode of directory to 334 --
bool(true)
bool(true)
-- Changing mode of directory to 335 --
bool(true)
bool(true)
-- Changing mode of directory to 336 --
bool(true)
bool(true)
-- Changing mode of directory to 337 --
bool(true)
bool(true)
-- Changing mode of directory to 338 --
bool(true)
bool(true)
-- Changing mode of directory to 339 --
bool(true)
bool(true)
-- Changing mode of directory to 340 --
bool(true)
bool(true)
-- Changing mode of directory to 341 --
bool(true)
bool(true)
-- Changing mode of directory to 342 --
bool(true)
bool(true)
-- Changing mode of directory to 343 --
bool(true)
bool(true)
-- Changing mode of directory to 344 --
bool(true)
bool(true)
-- Changing mode of directory to 345 --
bool(true)
bool(true)
-- Changing mode of directory to 346 --
bool(true)
bool(true)
-- Changing mode of directory to 347 --
bool(true)
bool(true)
-- Changing mode of directory to 348 --
bool(true)
bool(true)
-- Changing mode of directory to 349 --
bool(true)
bool(true)
-- Changing mode of directory to 350 --
bool(true)
bool(true)
-- Changing mode of directory to 351 --
bool(true)
bool(true)
-- Changing mode of directory to 352 --
bool(true)
bool(true)
-- Changing mode of directory to 353 --
bool(true)
bool(true)
-- Changing mode of directory to 354 --
bool(true)
bool(true)
-- Changing mode of directory to 355 --
bool(true)
bool(true)
-- Changing mode of directory to 356 --
bool(true)
bool(true)
-- Changing mode of directory to 357 --
bool(true)
bool(true)
-- Changing mode of directory to 358 --
bool(true)
bool(true)
-- Changing mode of directory to 359 --
bool(true)
bool(true)
-- Changing mode of directory to 360 --
bool(true)
bool(true)
-- Changing mode of directory to 361 --
bool(true)
bool(true)
-- Changing mode of directory to 362 --
bool(true)
bool(true)
-- Changing mode of directory to 363 --
bool(true)
bool(true)
-- Changing mode of directory to 364 --
bool(true)
bool(true)
-- Changing mode of directory to 365 --
bool(true)
bool(true)
-- Changing mode of directory to 366 --
bool(true)
bool(true)
-- Changing mode of directory to 367 --
bool(true)
bool(true)
-- Changing mode of directory to 368 --
bool(true)
bool(true)
-- Changing mode of directory to 369 --
bool(true)
bool(true)
-- Changing mode of directory to 370 --
bool(true)
bool(true)
-- Changing mode of directory to 371 --
bool(true)
bool(true)
-- Changing mode of directory to 372 --
bool(true)
bool(true)
-- Changing mode of directory to 373 --
bool(true)
bool(true)
-- Changing mode of directory to 374 --
bool(true)
bool(true)
-- Changing mode of directory to 375 --
bool(true)
bool(true)
-- Changing mode of directory to 376 --
bool(true)
bool(true)
-- Changing mode of directory to 377 --
bool(true)
bool(true)
-- Changing mode of directory to 378 --
bool(true)
bool(true)
-- Changing mode of directory to 379 --
bool(true)
bool(true)
-- Changing mode of directory to 380 --
bool(true)
bool(true)
-- Changing mode of directory to 381 --
bool(true)
bool(true)
-- Changing mode of directory to 382 --
bool(true)
bool(true)
-- Changing mode of directory to 383 --
bool(true)
bool(true)
-- Changing mode of directory to 384 --
bool(true)
bool(true)
-- Changing mode of directory to 385 --
bool(true)
bool(true)
-- Changing mode of directory to 386 --
bool(true)
bool(true)
-- Changing mode of directory to 387 --
bool(true)
bool(true)
-- Changing mode of directory to 388 --
bool(true)
bool(true)
-- Changing mode of directory to 389 --
bool(true)
bool(true)
-- Changing mode of directory to 390 --
bool(true)
bool(true)
-- Changing mode of directory to 391 --
bool(true)
bool(true)
-- Changing mode of directory to 392 --
bool(true)
bool(true)
-- Changing mode of directory to 393 --
bool(true)
bool(true)
-- Changing mode of directory to 394 --
bool(true)
bool(true)
-- Changing mode of directory to 395 --
bool(true)
bool(true)
-- Changing mode of directory to 396 --
bool(true)
bool(true)
-- Changing mode of directory to 397 --
bool(true)
bool(true)
-- Changing mode of directory to 398 --
bool(true)
bool(true)
-- Changing mode of directory to 399 --
bool(true)
bool(true)
-- Changing mode of directory to 400 --
bool(true)
bool(true)
-- Changing mode of directory to 401 --
bool(true)
bool(true)
-- Changing mode of directory to 402 --
bool(true)
bool(true)
-- Changing mode of directory to 403 --
bool(true)
bool(true)
-- Changing mode of directory to 404 --
bool(true)
bool(true)
-- Changing mode of directory to 405 --
bool(true)
bool(true)
-- Changing mode of directory to 406 --
bool(true)
bool(true)
-- Changing mode of directory to 407 --
bool(true)
bool(true)
-- Changing mode of directory to 408 --
bool(true)
bool(true)
-- Changing mode of directory to 409 --
bool(true)
bool(true)
-- Changing mode of directory to 410 --
bool(true)
bool(true)
-- Changing mode of directory to 411 --
bool(true)
bool(true)
-- Changing mode of directory to 412 --
bool(true)
bool(true)
-- Changing mode of directory to 413 --
bool(true)
bool(true)
-- Changing mode of directory to 414 --
bool(true)
bool(true)
-- Changing mode of directory to 415 --
bool(true)
bool(true)
-- Changing mode of directory to 416 --
bool(true)
bool(true)
-- Changing mode of directory to 417 --
bool(true)
bool(true)
-- Changing mode of directory to 418 --
bool(true)
bool(true)
-- Changing mode of directory to 419 --
bool(true)
bool(true)
-- Changing mode of directory to 420 --
bool(true)
bool(true)
-- Changing mode of directory to 421 --
bool(true)
bool(true)
-- Changing mode of directory to 422 --
bool(true)
bool(true)
-- Changing mode of directory to 423 --
bool(true)
bool(true)
-- Changing mode of directory to 424 --
bool(true)
bool(true)
-- Changing mode of directory to 425 --
bool(true)
bool(true)
-- Changing mode of directory to 426 --
bool(true)
bool(true)
-- Changing mode of directory to 427 --
bool(true)
bool(true)
-- Changing mode of directory to 428 --
bool(true)
bool(true)
-- Changing mode of directory to 429 --
bool(true)
bool(true)
-- Changing mode of directory to 430 --
bool(true)
bool(true)
-- Changing mode of directory to 431 --
bool(true)
bool(true)
-- Changing mode of directory to 432 --
bool(true)
bool(true)
-- Changing mode of directory to 433 --
bool(true)
bool(true)
-- Changing mode of directory to 434 --
bool(true)
bool(true)
-- Changing mode of directory to 435 --
bool(true)
bool(true)
-- Changing mode of directory to 436 --
bool(true)
bool(true)
-- Changing mode of directory to 437 --
bool(true)
bool(true)
-- Changing mode of directory to 438 --
bool(true)
bool(true)
-- Changing mode of directory to 439 --
bool(true)
bool(true)
-- Changing mode of directory to 440 --
bool(true)
bool(true)
-- Changing mode of directory to 441 --
bool(true)
bool(true)
-- Changing mode of directory to 442 --
bool(true)
bool(true)
-- Changing mode of directory to 443 --
bool(true)
bool(true)
-- Changing mode of directory to 444 --
bool(true)
bool(true)
-- Changing mode of directory to 445 --
bool(true)
bool(true)
-- Changing mode of directory to 446 --
bool(true)
bool(true)
-- Changing mode of directory to 447 --
bool(true)
bool(true)
-- Changing mode of directory to 448 --
bool(true)
bool(true)
-- Changing mode of directory to 449 --
bool(true)
bool(true)
-- Changing mode of directory to 450 --
bool(true)
bool(true)
-- Changing mode of directory to 451 --
bool(true)
bool(true)
-- Changing mode of directory to 452 --
bool(true)
bool(true)
-- Changing mode of directory to 453 --
bool(true)
bool(true)
-- Changing mode of directory to 454 --
bool(true)
bool(true)
-- Changing mode of directory to 455 --
bool(true)
bool(true)
-- Changing mode of directory to 456 --
bool(true)
bool(true)
-- Changing mode of directory to 457 --
bool(true)
bool(true)
-- Changing mode of directory to 458 --
bool(true)
bool(true)
-- Changing mode of directory to 459 --
bool(true)
bool(true)
-- Changing mode of directory to 460 --
bool(true)
bool(true)
-- Changing mode of directory to 461 --
bool(true)
bool(true)
-- Changing mode of directory to 462 --
bool(true)
bool(true)
-- Changing mode of directory to 463 --
bool(true)
bool(true)
-- Changing mode of directory to 464 --
bool(true)
bool(true)
-- Changing mode of directory to 465 --
bool(true)
bool(true)
-- Changing mode of directory to 466 --
bool(true)
bool(true)
-- Changing mode of directory to 467 --
bool(true)
bool(true)
-- Changing mode of directory to 468 --
bool(true)
bool(true)
-- Changing mode of directory to 469 --
bool(true)
bool(true)
-- Changing mode of directory to 470 --
bool(true)
bool(true)
-- Changing mode of directory to 471 --
bool(true)
bool(true)
-- Changing mode of directory to 472 --
bool(true)
bool(true)
-- Changing mode of directory to 473 --
bool(true)
bool(true)
-- Changing mode of directory to 474 --
bool(true)
bool(true)
-- Changing mode of directory to 475 --
bool(true)
bool(true)
-- Changing mode of directory to 476 --
bool(true)
bool(true)
-- Changing mode of directory to 477 --
bool(true)
bool(true)
-- Changing mode of directory to 478 --
bool(true)
bool(true)
-- Changing mode of directory to 479 --
bool(true)
bool(true)
-- Changing mode of directory to 480 --
bool(true)
bool(true)
-- Changing mode of directory to 481 --
bool(true)
bool(true)
-- Changing mode of directory to 482 --
bool(true)
bool(true)
-- Changing mode of directory to 483 --
bool(true)
bool(true)
-- Changing mode of directory to 484 --
bool(true)
bool(true)
-- Changing mode of directory to 485 --
bool(true)
bool(true)
-- Changing mode of directory to 486 --
bool(true)
bool(true)
-- Changing mode of directory to 487 --
bool(true)
bool(true)
-- Changing mode of directory to 488 --
bool(true)
bool(true)
-- Changing mode of directory to 489 --
bool(true)
bool(true)
-- Changing mode of directory to 490 --
bool(true)
bool(true)
-- Changing mode of directory to 491 --
bool(true)
bool(true)
-- Changing mode of directory to 492 --
bool(true)
bool(true)
-- Changing mode of directory to 493 --
bool(true)
bool(true)
-- Changing mode of directory to 494 --
bool(true)
bool(true)
-- Changing mode of directory to 495 --
bool(true)
bool(true)
-- Changing mode of directory to 496 --
bool(true)
bool(true)
-- Changing mode of directory to 497 --
bool(true)
bool(true)
-- Changing mode of directory to 498 --
bool(true)
bool(true)
-- Changing mode of directory to 499 --
bool(true)
bool(true)
-- Changing mode of directory to 500 --
bool(true)
bool(true)
-- Changing mode of directory to 501 --
bool(true)
bool(true)
-- Changing mode of directory to 502 --
bool(true)
bool(true)
-- Changing mode of directory to 503 --
bool(true)
bool(true)
-- Changing mode of directory to 504 --
bool(true)
bool(true)
-- Changing mode of directory to 505 --
bool(true)
bool(true)
-- Changing mode of directory to 506 --
bool(true)
bool(true)
-- Changing mode of directory to 507 --
bool(true)
bool(true)
-- Changing mode of directory to 508 --
bool(true)
bool(true)
-- Changing mode of directory to 509 --
bool(true)
bool(true)
-- Changing mode of directory to 510 --
bool(true)
bool(true)
-- Changing mode of directory to 511 --
bool(true)
bool(true)
Done
