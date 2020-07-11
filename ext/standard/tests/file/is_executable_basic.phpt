--TEST--
Test is_executable() function: basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip not for windows');
}
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
// include common file test functions
require __DIR__.'/file.inc';

echo "*** Testing is_executable(): basic functionality ***\n";

// create a file
$filename = __DIR__."/is_executable.tmp";
create_file($filename);

$counter = 1;
/* loop to check if the file with new mode is executable
   using is_executable() */
for($mode = 0000; $mode <= 0777; $mode++) {
  echo "-- Changing mode of file to $mode --\n";
  chmod($filename, $mode);  // change mode of file
  var_dump( is_executable($filename) );
  $counter++;
  clearstatcache();
}

// delete the temp file
delete_file($filename);

echo "Done\n";
?>
--EXPECT--
*** Testing is_executable(): basic functionality ***
-- Changing mode of file to 0 --
bool(false)
-- Changing mode of file to 1 --
bool(false)
-- Changing mode of file to 2 --
bool(false)
-- Changing mode of file to 3 --
bool(false)
-- Changing mode of file to 4 --
bool(false)
-- Changing mode of file to 5 --
bool(false)
-- Changing mode of file to 6 --
bool(false)
-- Changing mode of file to 7 --
bool(false)
-- Changing mode of file to 8 --
bool(false)
-- Changing mode of file to 9 --
bool(false)
-- Changing mode of file to 10 --
bool(false)
-- Changing mode of file to 11 --
bool(false)
-- Changing mode of file to 12 --
bool(false)
-- Changing mode of file to 13 --
bool(false)
-- Changing mode of file to 14 --
bool(false)
-- Changing mode of file to 15 --
bool(false)
-- Changing mode of file to 16 --
bool(false)
-- Changing mode of file to 17 --
bool(false)
-- Changing mode of file to 18 --
bool(false)
-- Changing mode of file to 19 --
bool(false)
-- Changing mode of file to 20 --
bool(false)
-- Changing mode of file to 21 --
bool(false)
-- Changing mode of file to 22 --
bool(false)
-- Changing mode of file to 23 --
bool(false)
-- Changing mode of file to 24 --
bool(false)
-- Changing mode of file to 25 --
bool(false)
-- Changing mode of file to 26 --
bool(false)
-- Changing mode of file to 27 --
bool(false)
-- Changing mode of file to 28 --
bool(false)
-- Changing mode of file to 29 --
bool(false)
-- Changing mode of file to 30 --
bool(false)
-- Changing mode of file to 31 --
bool(false)
-- Changing mode of file to 32 --
bool(false)
-- Changing mode of file to 33 --
bool(false)
-- Changing mode of file to 34 --
bool(false)
-- Changing mode of file to 35 --
bool(false)
-- Changing mode of file to 36 --
bool(false)
-- Changing mode of file to 37 --
bool(false)
-- Changing mode of file to 38 --
bool(false)
-- Changing mode of file to 39 --
bool(false)
-- Changing mode of file to 40 --
bool(false)
-- Changing mode of file to 41 --
bool(false)
-- Changing mode of file to 42 --
bool(false)
-- Changing mode of file to 43 --
bool(false)
-- Changing mode of file to 44 --
bool(false)
-- Changing mode of file to 45 --
bool(false)
-- Changing mode of file to 46 --
bool(false)
-- Changing mode of file to 47 --
bool(false)
-- Changing mode of file to 48 --
bool(false)
-- Changing mode of file to 49 --
bool(false)
-- Changing mode of file to 50 --
bool(false)
-- Changing mode of file to 51 --
bool(false)
-- Changing mode of file to 52 --
bool(false)
-- Changing mode of file to 53 --
bool(false)
-- Changing mode of file to 54 --
bool(false)
-- Changing mode of file to 55 --
bool(false)
-- Changing mode of file to 56 --
bool(false)
-- Changing mode of file to 57 --
bool(false)
-- Changing mode of file to 58 --
bool(false)
-- Changing mode of file to 59 --
bool(false)
-- Changing mode of file to 60 --
bool(false)
-- Changing mode of file to 61 --
bool(false)
-- Changing mode of file to 62 --
bool(false)
-- Changing mode of file to 63 --
bool(false)
-- Changing mode of file to 64 --
bool(true)
-- Changing mode of file to 65 --
bool(true)
-- Changing mode of file to 66 --
bool(true)
-- Changing mode of file to 67 --
bool(true)
-- Changing mode of file to 68 --
bool(true)
-- Changing mode of file to 69 --
bool(true)
-- Changing mode of file to 70 --
bool(true)
-- Changing mode of file to 71 --
bool(true)
-- Changing mode of file to 72 --
bool(true)
-- Changing mode of file to 73 --
bool(true)
-- Changing mode of file to 74 --
bool(true)
-- Changing mode of file to 75 --
bool(true)
-- Changing mode of file to 76 --
bool(true)
-- Changing mode of file to 77 --
bool(true)
-- Changing mode of file to 78 --
bool(true)
-- Changing mode of file to 79 --
bool(true)
-- Changing mode of file to 80 --
bool(true)
-- Changing mode of file to 81 --
bool(true)
-- Changing mode of file to 82 --
bool(true)
-- Changing mode of file to 83 --
bool(true)
-- Changing mode of file to 84 --
bool(true)
-- Changing mode of file to 85 --
bool(true)
-- Changing mode of file to 86 --
bool(true)
-- Changing mode of file to 87 --
bool(true)
-- Changing mode of file to 88 --
bool(true)
-- Changing mode of file to 89 --
bool(true)
-- Changing mode of file to 90 --
bool(true)
-- Changing mode of file to 91 --
bool(true)
-- Changing mode of file to 92 --
bool(true)
-- Changing mode of file to 93 --
bool(true)
-- Changing mode of file to 94 --
bool(true)
-- Changing mode of file to 95 --
bool(true)
-- Changing mode of file to 96 --
bool(true)
-- Changing mode of file to 97 --
bool(true)
-- Changing mode of file to 98 --
bool(true)
-- Changing mode of file to 99 --
bool(true)
-- Changing mode of file to 100 --
bool(true)
-- Changing mode of file to 101 --
bool(true)
-- Changing mode of file to 102 --
bool(true)
-- Changing mode of file to 103 --
bool(true)
-- Changing mode of file to 104 --
bool(true)
-- Changing mode of file to 105 --
bool(true)
-- Changing mode of file to 106 --
bool(true)
-- Changing mode of file to 107 --
bool(true)
-- Changing mode of file to 108 --
bool(true)
-- Changing mode of file to 109 --
bool(true)
-- Changing mode of file to 110 --
bool(true)
-- Changing mode of file to 111 --
bool(true)
-- Changing mode of file to 112 --
bool(true)
-- Changing mode of file to 113 --
bool(true)
-- Changing mode of file to 114 --
bool(true)
-- Changing mode of file to 115 --
bool(true)
-- Changing mode of file to 116 --
bool(true)
-- Changing mode of file to 117 --
bool(true)
-- Changing mode of file to 118 --
bool(true)
-- Changing mode of file to 119 --
bool(true)
-- Changing mode of file to 120 --
bool(true)
-- Changing mode of file to 121 --
bool(true)
-- Changing mode of file to 122 --
bool(true)
-- Changing mode of file to 123 --
bool(true)
-- Changing mode of file to 124 --
bool(true)
-- Changing mode of file to 125 --
bool(true)
-- Changing mode of file to 126 --
bool(true)
-- Changing mode of file to 127 --
bool(true)
-- Changing mode of file to 128 --
bool(false)
-- Changing mode of file to 129 --
bool(false)
-- Changing mode of file to 130 --
bool(false)
-- Changing mode of file to 131 --
bool(false)
-- Changing mode of file to 132 --
bool(false)
-- Changing mode of file to 133 --
bool(false)
-- Changing mode of file to 134 --
bool(false)
-- Changing mode of file to 135 --
bool(false)
-- Changing mode of file to 136 --
bool(false)
-- Changing mode of file to 137 --
bool(false)
-- Changing mode of file to 138 --
bool(false)
-- Changing mode of file to 139 --
bool(false)
-- Changing mode of file to 140 --
bool(false)
-- Changing mode of file to 141 --
bool(false)
-- Changing mode of file to 142 --
bool(false)
-- Changing mode of file to 143 --
bool(false)
-- Changing mode of file to 144 --
bool(false)
-- Changing mode of file to 145 --
bool(false)
-- Changing mode of file to 146 --
bool(false)
-- Changing mode of file to 147 --
bool(false)
-- Changing mode of file to 148 --
bool(false)
-- Changing mode of file to 149 --
bool(false)
-- Changing mode of file to 150 --
bool(false)
-- Changing mode of file to 151 --
bool(false)
-- Changing mode of file to 152 --
bool(false)
-- Changing mode of file to 153 --
bool(false)
-- Changing mode of file to 154 --
bool(false)
-- Changing mode of file to 155 --
bool(false)
-- Changing mode of file to 156 --
bool(false)
-- Changing mode of file to 157 --
bool(false)
-- Changing mode of file to 158 --
bool(false)
-- Changing mode of file to 159 --
bool(false)
-- Changing mode of file to 160 --
bool(false)
-- Changing mode of file to 161 --
bool(false)
-- Changing mode of file to 162 --
bool(false)
-- Changing mode of file to 163 --
bool(false)
-- Changing mode of file to 164 --
bool(false)
-- Changing mode of file to 165 --
bool(false)
-- Changing mode of file to 166 --
bool(false)
-- Changing mode of file to 167 --
bool(false)
-- Changing mode of file to 168 --
bool(false)
-- Changing mode of file to 169 --
bool(false)
-- Changing mode of file to 170 --
bool(false)
-- Changing mode of file to 171 --
bool(false)
-- Changing mode of file to 172 --
bool(false)
-- Changing mode of file to 173 --
bool(false)
-- Changing mode of file to 174 --
bool(false)
-- Changing mode of file to 175 --
bool(false)
-- Changing mode of file to 176 --
bool(false)
-- Changing mode of file to 177 --
bool(false)
-- Changing mode of file to 178 --
bool(false)
-- Changing mode of file to 179 --
bool(false)
-- Changing mode of file to 180 --
bool(false)
-- Changing mode of file to 181 --
bool(false)
-- Changing mode of file to 182 --
bool(false)
-- Changing mode of file to 183 --
bool(false)
-- Changing mode of file to 184 --
bool(false)
-- Changing mode of file to 185 --
bool(false)
-- Changing mode of file to 186 --
bool(false)
-- Changing mode of file to 187 --
bool(false)
-- Changing mode of file to 188 --
bool(false)
-- Changing mode of file to 189 --
bool(false)
-- Changing mode of file to 190 --
bool(false)
-- Changing mode of file to 191 --
bool(false)
-- Changing mode of file to 192 --
bool(true)
-- Changing mode of file to 193 --
bool(true)
-- Changing mode of file to 194 --
bool(true)
-- Changing mode of file to 195 --
bool(true)
-- Changing mode of file to 196 --
bool(true)
-- Changing mode of file to 197 --
bool(true)
-- Changing mode of file to 198 --
bool(true)
-- Changing mode of file to 199 --
bool(true)
-- Changing mode of file to 200 --
bool(true)
-- Changing mode of file to 201 --
bool(true)
-- Changing mode of file to 202 --
bool(true)
-- Changing mode of file to 203 --
bool(true)
-- Changing mode of file to 204 --
bool(true)
-- Changing mode of file to 205 --
bool(true)
-- Changing mode of file to 206 --
bool(true)
-- Changing mode of file to 207 --
bool(true)
-- Changing mode of file to 208 --
bool(true)
-- Changing mode of file to 209 --
bool(true)
-- Changing mode of file to 210 --
bool(true)
-- Changing mode of file to 211 --
bool(true)
-- Changing mode of file to 212 --
bool(true)
-- Changing mode of file to 213 --
bool(true)
-- Changing mode of file to 214 --
bool(true)
-- Changing mode of file to 215 --
bool(true)
-- Changing mode of file to 216 --
bool(true)
-- Changing mode of file to 217 --
bool(true)
-- Changing mode of file to 218 --
bool(true)
-- Changing mode of file to 219 --
bool(true)
-- Changing mode of file to 220 --
bool(true)
-- Changing mode of file to 221 --
bool(true)
-- Changing mode of file to 222 --
bool(true)
-- Changing mode of file to 223 --
bool(true)
-- Changing mode of file to 224 --
bool(true)
-- Changing mode of file to 225 --
bool(true)
-- Changing mode of file to 226 --
bool(true)
-- Changing mode of file to 227 --
bool(true)
-- Changing mode of file to 228 --
bool(true)
-- Changing mode of file to 229 --
bool(true)
-- Changing mode of file to 230 --
bool(true)
-- Changing mode of file to 231 --
bool(true)
-- Changing mode of file to 232 --
bool(true)
-- Changing mode of file to 233 --
bool(true)
-- Changing mode of file to 234 --
bool(true)
-- Changing mode of file to 235 --
bool(true)
-- Changing mode of file to 236 --
bool(true)
-- Changing mode of file to 237 --
bool(true)
-- Changing mode of file to 238 --
bool(true)
-- Changing mode of file to 239 --
bool(true)
-- Changing mode of file to 240 --
bool(true)
-- Changing mode of file to 241 --
bool(true)
-- Changing mode of file to 242 --
bool(true)
-- Changing mode of file to 243 --
bool(true)
-- Changing mode of file to 244 --
bool(true)
-- Changing mode of file to 245 --
bool(true)
-- Changing mode of file to 246 --
bool(true)
-- Changing mode of file to 247 --
bool(true)
-- Changing mode of file to 248 --
bool(true)
-- Changing mode of file to 249 --
bool(true)
-- Changing mode of file to 250 --
bool(true)
-- Changing mode of file to 251 --
bool(true)
-- Changing mode of file to 252 --
bool(true)
-- Changing mode of file to 253 --
bool(true)
-- Changing mode of file to 254 --
bool(true)
-- Changing mode of file to 255 --
bool(true)
-- Changing mode of file to 256 --
bool(false)
-- Changing mode of file to 257 --
bool(false)
-- Changing mode of file to 258 --
bool(false)
-- Changing mode of file to 259 --
bool(false)
-- Changing mode of file to 260 --
bool(false)
-- Changing mode of file to 261 --
bool(false)
-- Changing mode of file to 262 --
bool(false)
-- Changing mode of file to 263 --
bool(false)
-- Changing mode of file to 264 --
bool(false)
-- Changing mode of file to 265 --
bool(false)
-- Changing mode of file to 266 --
bool(false)
-- Changing mode of file to 267 --
bool(false)
-- Changing mode of file to 268 --
bool(false)
-- Changing mode of file to 269 --
bool(false)
-- Changing mode of file to 270 --
bool(false)
-- Changing mode of file to 271 --
bool(false)
-- Changing mode of file to 272 --
bool(false)
-- Changing mode of file to 273 --
bool(false)
-- Changing mode of file to 274 --
bool(false)
-- Changing mode of file to 275 --
bool(false)
-- Changing mode of file to 276 --
bool(false)
-- Changing mode of file to 277 --
bool(false)
-- Changing mode of file to 278 --
bool(false)
-- Changing mode of file to 279 --
bool(false)
-- Changing mode of file to 280 --
bool(false)
-- Changing mode of file to 281 --
bool(false)
-- Changing mode of file to 282 --
bool(false)
-- Changing mode of file to 283 --
bool(false)
-- Changing mode of file to 284 --
bool(false)
-- Changing mode of file to 285 --
bool(false)
-- Changing mode of file to 286 --
bool(false)
-- Changing mode of file to 287 --
bool(false)
-- Changing mode of file to 288 --
bool(false)
-- Changing mode of file to 289 --
bool(false)
-- Changing mode of file to 290 --
bool(false)
-- Changing mode of file to 291 --
bool(false)
-- Changing mode of file to 292 --
bool(false)
-- Changing mode of file to 293 --
bool(false)
-- Changing mode of file to 294 --
bool(false)
-- Changing mode of file to 295 --
bool(false)
-- Changing mode of file to 296 --
bool(false)
-- Changing mode of file to 297 --
bool(false)
-- Changing mode of file to 298 --
bool(false)
-- Changing mode of file to 299 --
bool(false)
-- Changing mode of file to 300 --
bool(false)
-- Changing mode of file to 301 --
bool(false)
-- Changing mode of file to 302 --
bool(false)
-- Changing mode of file to 303 --
bool(false)
-- Changing mode of file to 304 --
bool(false)
-- Changing mode of file to 305 --
bool(false)
-- Changing mode of file to 306 --
bool(false)
-- Changing mode of file to 307 --
bool(false)
-- Changing mode of file to 308 --
bool(false)
-- Changing mode of file to 309 --
bool(false)
-- Changing mode of file to 310 --
bool(false)
-- Changing mode of file to 311 --
bool(false)
-- Changing mode of file to 312 --
bool(false)
-- Changing mode of file to 313 --
bool(false)
-- Changing mode of file to 314 --
bool(false)
-- Changing mode of file to 315 --
bool(false)
-- Changing mode of file to 316 --
bool(false)
-- Changing mode of file to 317 --
bool(false)
-- Changing mode of file to 318 --
bool(false)
-- Changing mode of file to 319 --
bool(false)
-- Changing mode of file to 320 --
bool(true)
-- Changing mode of file to 321 --
bool(true)
-- Changing mode of file to 322 --
bool(true)
-- Changing mode of file to 323 --
bool(true)
-- Changing mode of file to 324 --
bool(true)
-- Changing mode of file to 325 --
bool(true)
-- Changing mode of file to 326 --
bool(true)
-- Changing mode of file to 327 --
bool(true)
-- Changing mode of file to 328 --
bool(true)
-- Changing mode of file to 329 --
bool(true)
-- Changing mode of file to 330 --
bool(true)
-- Changing mode of file to 331 --
bool(true)
-- Changing mode of file to 332 --
bool(true)
-- Changing mode of file to 333 --
bool(true)
-- Changing mode of file to 334 --
bool(true)
-- Changing mode of file to 335 --
bool(true)
-- Changing mode of file to 336 --
bool(true)
-- Changing mode of file to 337 --
bool(true)
-- Changing mode of file to 338 --
bool(true)
-- Changing mode of file to 339 --
bool(true)
-- Changing mode of file to 340 --
bool(true)
-- Changing mode of file to 341 --
bool(true)
-- Changing mode of file to 342 --
bool(true)
-- Changing mode of file to 343 --
bool(true)
-- Changing mode of file to 344 --
bool(true)
-- Changing mode of file to 345 --
bool(true)
-- Changing mode of file to 346 --
bool(true)
-- Changing mode of file to 347 --
bool(true)
-- Changing mode of file to 348 --
bool(true)
-- Changing mode of file to 349 --
bool(true)
-- Changing mode of file to 350 --
bool(true)
-- Changing mode of file to 351 --
bool(true)
-- Changing mode of file to 352 --
bool(true)
-- Changing mode of file to 353 --
bool(true)
-- Changing mode of file to 354 --
bool(true)
-- Changing mode of file to 355 --
bool(true)
-- Changing mode of file to 356 --
bool(true)
-- Changing mode of file to 357 --
bool(true)
-- Changing mode of file to 358 --
bool(true)
-- Changing mode of file to 359 --
bool(true)
-- Changing mode of file to 360 --
bool(true)
-- Changing mode of file to 361 --
bool(true)
-- Changing mode of file to 362 --
bool(true)
-- Changing mode of file to 363 --
bool(true)
-- Changing mode of file to 364 --
bool(true)
-- Changing mode of file to 365 --
bool(true)
-- Changing mode of file to 366 --
bool(true)
-- Changing mode of file to 367 --
bool(true)
-- Changing mode of file to 368 --
bool(true)
-- Changing mode of file to 369 --
bool(true)
-- Changing mode of file to 370 --
bool(true)
-- Changing mode of file to 371 --
bool(true)
-- Changing mode of file to 372 --
bool(true)
-- Changing mode of file to 373 --
bool(true)
-- Changing mode of file to 374 --
bool(true)
-- Changing mode of file to 375 --
bool(true)
-- Changing mode of file to 376 --
bool(true)
-- Changing mode of file to 377 --
bool(true)
-- Changing mode of file to 378 --
bool(true)
-- Changing mode of file to 379 --
bool(true)
-- Changing mode of file to 380 --
bool(true)
-- Changing mode of file to 381 --
bool(true)
-- Changing mode of file to 382 --
bool(true)
-- Changing mode of file to 383 --
bool(true)
-- Changing mode of file to 384 --
bool(false)
-- Changing mode of file to 385 --
bool(false)
-- Changing mode of file to 386 --
bool(false)
-- Changing mode of file to 387 --
bool(false)
-- Changing mode of file to 388 --
bool(false)
-- Changing mode of file to 389 --
bool(false)
-- Changing mode of file to 390 --
bool(false)
-- Changing mode of file to 391 --
bool(false)
-- Changing mode of file to 392 --
bool(false)
-- Changing mode of file to 393 --
bool(false)
-- Changing mode of file to 394 --
bool(false)
-- Changing mode of file to 395 --
bool(false)
-- Changing mode of file to 396 --
bool(false)
-- Changing mode of file to 397 --
bool(false)
-- Changing mode of file to 398 --
bool(false)
-- Changing mode of file to 399 --
bool(false)
-- Changing mode of file to 400 --
bool(false)
-- Changing mode of file to 401 --
bool(false)
-- Changing mode of file to 402 --
bool(false)
-- Changing mode of file to 403 --
bool(false)
-- Changing mode of file to 404 --
bool(false)
-- Changing mode of file to 405 --
bool(false)
-- Changing mode of file to 406 --
bool(false)
-- Changing mode of file to 407 --
bool(false)
-- Changing mode of file to 408 --
bool(false)
-- Changing mode of file to 409 --
bool(false)
-- Changing mode of file to 410 --
bool(false)
-- Changing mode of file to 411 --
bool(false)
-- Changing mode of file to 412 --
bool(false)
-- Changing mode of file to 413 --
bool(false)
-- Changing mode of file to 414 --
bool(false)
-- Changing mode of file to 415 --
bool(false)
-- Changing mode of file to 416 --
bool(false)
-- Changing mode of file to 417 --
bool(false)
-- Changing mode of file to 418 --
bool(false)
-- Changing mode of file to 419 --
bool(false)
-- Changing mode of file to 420 --
bool(false)
-- Changing mode of file to 421 --
bool(false)
-- Changing mode of file to 422 --
bool(false)
-- Changing mode of file to 423 --
bool(false)
-- Changing mode of file to 424 --
bool(false)
-- Changing mode of file to 425 --
bool(false)
-- Changing mode of file to 426 --
bool(false)
-- Changing mode of file to 427 --
bool(false)
-- Changing mode of file to 428 --
bool(false)
-- Changing mode of file to 429 --
bool(false)
-- Changing mode of file to 430 --
bool(false)
-- Changing mode of file to 431 --
bool(false)
-- Changing mode of file to 432 --
bool(false)
-- Changing mode of file to 433 --
bool(false)
-- Changing mode of file to 434 --
bool(false)
-- Changing mode of file to 435 --
bool(false)
-- Changing mode of file to 436 --
bool(false)
-- Changing mode of file to 437 --
bool(false)
-- Changing mode of file to 438 --
bool(false)
-- Changing mode of file to 439 --
bool(false)
-- Changing mode of file to 440 --
bool(false)
-- Changing mode of file to 441 --
bool(false)
-- Changing mode of file to 442 --
bool(false)
-- Changing mode of file to 443 --
bool(false)
-- Changing mode of file to 444 --
bool(false)
-- Changing mode of file to 445 --
bool(false)
-- Changing mode of file to 446 --
bool(false)
-- Changing mode of file to 447 --
bool(false)
-- Changing mode of file to 448 --
bool(true)
-- Changing mode of file to 449 --
bool(true)
-- Changing mode of file to 450 --
bool(true)
-- Changing mode of file to 451 --
bool(true)
-- Changing mode of file to 452 --
bool(true)
-- Changing mode of file to 453 --
bool(true)
-- Changing mode of file to 454 --
bool(true)
-- Changing mode of file to 455 --
bool(true)
-- Changing mode of file to 456 --
bool(true)
-- Changing mode of file to 457 --
bool(true)
-- Changing mode of file to 458 --
bool(true)
-- Changing mode of file to 459 --
bool(true)
-- Changing mode of file to 460 --
bool(true)
-- Changing mode of file to 461 --
bool(true)
-- Changing mode of file to 462 --
bool(true)
-- Changing mode of file to 463 --
bool(true)
-- Changing mode of file to 464 --
bool(true)
-- Changing mode of file to 465 --
bool(true)
-- Changing mode of file to 466 --
bool(true)
-- Changing mode of file to 467 --
bool(true)
-- Changing mode of file to 468 --
bool(true)
-- Changing mode of file to 469 --
bool(true)
-- Changing mode of file to 470 --
bool(true)
-- Changing mode of file to 471 --
bool(true)
-- Changing mode of file to 472 --
bool(true)
-- Changing mode of file to 473 --
bool(true)
-- Changing mode of file to 474 --
bool(true)
-- Changing mode of file to 475 --
bool(true)
-- Changing mode of file to 476 --
bool(true)
-- Changing mode of file to 477 --
bool(true)
-- Changing mode of file to 478 --
bool(true)
-- Changing mode of file to 479 --
bool(true)
-- Changing mode of file to 480 --
bool(true)
-- Changing mode of file to 481 --
bool(true)
-- Changing mode of file to 482 --
bool(true)
-- Changing mode of file to 483 --
bool(true)
-- Changing mode of file to 484 --
bool(true)
-- Changing mode of file to 485 --
bool(true)
-- Changing mode of file to 486 --
bool(true)
-- Changing mode of file to 487 --
bool(true)
-- Changing mode of file to 488 --
bool(true)
-- Changing mode of file to 489 --
bool(true)
-- Changing mode of file to 490 --
bool(true)
-- Changing mode of file to 491 --
bool(true)
-- Changing mode of file to 492 --
bool(true)
-- Changing mode of file to 493 --
bool(true)
-- Changing mode of file to 494 --
bool(true)
-- Changing mode of file to 495 --
bool(true)
-- Changing mode of file to 496 --
bool(true)
-- Changing mode of file to 497 --
bool(true)
-- Changing mode of file to 498 --
bool(true)
-- Changing mode of file to 499 --
bool(true)
-- Changing mode of file to 500 --
bool(true)
-- Changing mode of file to 501 --
bool(true)
-- Changing mode of file to 502 --
bool(true)
-- Changing mode of file to 503 --
bool(true)
-- Changing mode of file to 504 --
bool(true)
-- Changing mode of file to 505 --
bool(true)
-- Changing mode of file to 506 --
bool(true)
-- Changing mode of file to 507 --
bool(true)
-- Changing mode of file to 508 --
bool(true)
-- Changing mode of file to 509 --
bool(true)
-- Changing mode of file to 510 --
bool(true)
-- Changing mode of file to 511 --
bool(true)
Done
