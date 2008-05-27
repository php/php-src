--TEST--
Test unlink() function : usage variations - files with diff. file permissions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip only on Linux');
}
?>
--FILE--
<?php
/* Prototype : bool unlink ( string $filename [, resource $context] );
   Description : Deletes filename
*/

/* delete files with different file permission(0000 to 0777) */

$file_path = dirname(__FILE__);

// temp file used
$filename = "$file_path/unlink_variation5.tmp";

echo "*** Testing unlink() on a file ***\n";

for($mode = 0000; $mode <= 0777; $mode++ ) {
  // create temp file
  $fp = fopen($filename, "w");
  fclose($fp);
  // changing mode of file
  echo "File permission : $mode\n";
  var_dump( chmod($filename, $mode) );
  var_dump( unlink($filename) );  // deleting file
  var_dump( file_exists($filename) );  // confirm file deleted
}

echo "Done\n";
?>
--EXPECT--
*** Testing unlink() on a file ***
File permission : 0
bool(true)
bool(true)
bool(false)
File permission : 1
bool(true)
bool(true)
bool(false)
File permission : 2
bool(true)
bool(true)
bool(false)
File permission : 3
bool(true)
bool(true)
bool(false)
File permission : 4
bool(true)
bool(true)
bool(false)
File permission : 5
bool(true)
bool(true)
bool(false)
File permission : 6
bool(true)
bool(true)
bool(false)
File permission : 7
bool(true)
bool(true)
bool(false)
File permission : 8
bool(true)
bool(true)
bool(false)
File permission : 9
bool(true)
bool(true)
bool(false)
File permission : 10
bool(true)
bool(true)
bool(false)
File permission : 11
bool(true)
bool(true)
bool(false)
File permission : 12
bool(true)
bool(true)
bool(false)
File permission : 13
bool(true)
bool(true)
bool(false)
File permission : 14
bool(true)
bool(true)
bool(false)
File permission : 15
bool(true)
bool(true)
bool(false)
File permission : 16
bool(true)
bool(true)
bool(false)
File permission : 17
bool(true)
bool(true)
bool(false)
File permission : 18
bool(true)
bool(true)
bool(false)
File permission : 19
bool(true)
bool(true)
bool(false)
File permission : 20
bool(true)
bool(true)
bool(false)
File permission : 21
bool(true)
bool(true)
bool(false)
File permission : 22
bool(true)
bool(true)
bool(false)
File permission : 23
bool(true)
bool(true)
bool(false)
File permission : 24
bool(true)
bool(true)
bool(false)
File permission : 25
bool(true)
bool(true)
bool(false)
File permission : 26
bool(true)
bool(true)
bool(false)
File permission : 27
bool(true)
bool(true)
bool(false)
File permission : 28
bool(true)
bool(true)
bool(false)
File permission : 29
bool(true)
bool(true)
bool(false)
File permission : 30
bool(true)
bool(true)
bool(false)
File permission : 31
bool(true)
bool(true)
bool(false)
File permission : 32
bool(true)
bool(true)
bool(false)
File permission : 33
bool(true)
bool(true)
bool(false)
File permission : 34
bool(true)
bool(true)
bool(false)
File permission : 35
bool(true)
bool(true)
bool(false)
File permission : 36
bool(true)
bool(true)
bool(false)
File permission : 37
bool(true)
bool(true)
bool(false)
File permission : 38
bool(true)
bool(true)
bool(false)
File permission : 39
bool(true)
bool(true)
bool(false)
File permission : 40
bool(true)
bool(true)
bool(false)
File permission : 41
bool(true)
bool(true)
bool(false)
File permission : 42
bool(true)
bool(true)
bool(false)
File permission : 43
bool(true)
bool(true)
bool(false)
File permission : 44
bool(true)
bool(true)
bool(false)
File permission : 45
bool(true)
bool(true)
bool(false)
File permission : 46
bool(true)
bool(true)
bool(false)
File permission : 47
bool(true)
bool(true)
bool(false)
File permission : 48
bool(true)
bool(true)
bool(false)
File permission : 49
bool(true)
bool(true)
bool(false)
File permission : 50
bool(true)
bool(true)
bool(false)
File permission : 51
bool(true)
bool(true)
bool(false)
File permission : 52
bool(true)
bool(true)
bool(false)
File permission : 53
bool(true)
bool(true)
bool(false)
File permission : 54
bool(true)
bool(true)
bool(false)
File permission : 55
bool(true)
bool(true)
bool(false)
File permission : 56
bool(true)
bool(true)
bool(false)
File permission : 57
bool(true)
bool(true)
bool(false)
File permission : 58
bool(true)
bool(true)
bool(false)
File permission : 59
bool(true)
bool(true)
bool(false)
File permission : 60
bool(true)
bool(true)
bool(false)
File permission : 61
bool(true)
bool(true)
bool(false)
File permission : 62
bool(true)
bool(true)
bool(false)
File permission : 63
bool(true)
bool(true)
bool(false)
File permission : 64
bool(true)
bool(true)
bool(false)
File permission : 65
bool(true)
bool(true)
bool(false)
File permission : 66
bool(true)
bool(true)
bool(false)
File permission : 67
bool(true)
bool(true)
bool(false)
File permission : 68
bool(true)
bool(true)
bool(false)
File permission : 69
bool(true)
bool(true)
bool(false)
File permission : 70
bool(true)
bool(true)
bool(false)
File permission : 71
bool(true)
bool(true)
bool(false)
File permission : 72
bool(true)
bool(true)
bool(false)
File permission : 73
bool(true)
bool(true)
bool(false)
File permission : 74
bool(true)
bool(true)
bool(false)
File permission : 75
bool(true)
bool(true)
bool(false)
File permission : 76
bool(true)
bool(true)
bool(false)
File permission : 77
bool(true)
bool(true)
bool(false)
File permission : 78
bool(true)
bool(true)
bool(false)
File permission : 79
bool(true)
bool(true)
bool(false)
File permission : 80
bool(true)
bool(true)
bool(false)
File permission : 81
bool(true)
bool(true)
bool(false)
File permission : 82
bool(true)
bool(true)
bool(false)
File permission : 83
bool(true)
bool(true)
bool(false)
File permission : 84
bool(true)
bool(true)
bool(false)
File permission : 85
bool(true)
bool(true)
bool(false)
File permission : 86
bool(true)
bool(true)
bool(false)
File permission : 87
bool(true)
bool(true)
bool(false)
File permission : 88
bool(true)
bool(true)
bool(false)
File permission : 89
bool(true)
bool(true)
bool(false)
File permission : 90
bool(true)
bool(true)
bool(false)
File permission : 91
bool(true)
bool(true)
bool(false)
File permission : 92
bool(true)
bool(true)
bool(false)
File permission : 93
bool(true)
bool(true)
bool(false)
File permission : 94
bool(true)
bool(true)
bool(false)
File permission : 95
bool(true)
bool(true)
bool(false)
File permission : 96
bool(true)
bool(true)
bool(false)
File permission : 97
bool(true)
bool(true)
bool(false)
File permission : 98
bool(true)
bool(true)
bool(false)
File permission : 99
bool(true)
bool(true)
bool(false)
File permission : 100
bool(true)
bool(true)
bool(false)
File permission : 101
bool(true)
bool(true)
bool(false)
File permission : 102
bool(true)
bool(true)
bool(false)
File permission : 103
bool(true)
bool(true)
bool(false)
File permission : 104
bool(true)
bool(true)
bool(false)
File permission : 105
bool(true)
bool(true)
bool(false)
File permission : 106
bool(true)
bool(true)
bool(false)
File permission : 107
bool(true)
bool(true)
bool(false)
File permission : 108
bool(true)
bool(true)
bool(false)
File permission : 109
bool(true)
bool(true)
bool(false)
File permission : 110
bool(true)
bool(true)
bool(false)
File permission : 111
bool(true)
bool(true)
bool(false)
File permission : 112
bool(true)
bool(true)
bool(false)
File permission : 113
bool(true)
bool(true)
bool(false)
File permission : 114
bool(true)
bool(true)
bool(false)
File permission : 115
bool(true)
bool(true)
bool(false)
File permission : 116
bool(true)
bool(true)
bool(false)
File permission : 117
bool(true)
bool(true)
bool(false)
File permission : 118
bool(true)
bool(true)
bool(false)
File permission : 119
bool(true)
bool(true)
bool(false)
File permission : 120
bool(true)
bool(true)
bool(false)
File permission : 121
bool(true)
bool(true)
bool(false)
File permission : 122
bool(true)
bool(true)
bool(false)
File permission : 123
bool(true)
bool(true)
bool(false)
File permission : 124
bool(true)
bool(true)
bool(false)
File permission : 125
bool(true)
bool(true)
bool(false)
File permission : 126
bool(true)
bool(true)
bool(false)
File permission : 127
bool(true)
bool(true)
bool(false)
File permission : 128
bool(true)
bool(true)
bool(false)
File permission : 129
bool(true)
bool(true)
bool(false)
File permission : 130
bool(true)
bool(true)
bool(false)
File permission : 131
bool(true)
bool(true)
bool(false)
File permission : 132
bool(true)
bool(true)
bool(false)
File permission : 133
bool(true)
bool(true)
bool(false)
File permission : 134
bool(true)
bool(true)
bool(false)
File permission : 135
bool(true)
bool(true)
bool(false)
File permission : 136
bool(true)
bool(true)
bool(false)
File permission : 137
bool(true)
bool(true)
bool(false)
File permission : 138
bool(true)
bool(true)
bool(false)
File permission : 139
bool(true)
bool(true)
bool(false)
File permission : 140
bool(true)
bool(true)
bool(false)
File permission : 141
bool(true)
bool(true)
bool(false)
File permission : 142
bool(true)
bool(true)
bool(false)
File permission : 143
bool(true)
bool(true)
bool(false)
File permission : 144
bool(true)
bool(true)
bool(false)
File permission : 145
bool(true)
bool(true)
bool(false)
File permission : 146
bool(true)
bool(true)
bool(false)
File permission : 147
bool(true)
bool(true)
bool(false)
File permission : 148
bool(true)
bool(true)
bool(false)
File permission : 149
bool(true)
bool(true)
bool(false)
File permission : 150
bool(true)
bool(true)
bool(false)
File permission : 151
bool(true)
bool(true)
bool(false)
File permission : 152
bool(true)
bool(true)
bool(false)
File permission : 153
bool(true)
bool(true)
bool(false)
File permission : 154
bool(true)
bool(true)
bool(false)
File permission : 155
bool(true)
bool(true)
bool(false)
File permission : 156
bool(true)
bool(true)
bool(false)
File permission : 157
bool(true)
bool(true)
bool(false)
File permission : 158
bool(true)
bool(true)
bool(false)
File permission : 159
bool(true)
bool(true)
bool(false)
File permission : 160
bool(true)
bool(true)
bool(false)
File permission : 161
bool(true)
bool(true)
bool(false)
File permission : 162
bool(true)
bool(true)
bool(false)
File permission : 163
bool(true)
bool(true)
bool(false)
File permission : 164
bool(true)
bool(true)
bool(false)
File permission : 165
bool(true)
bool(true)
bool(false)
File permission : 166
bool(true)
bool(true)
bool(false)
File permission : 167
bool(true)
bool(true)
bool(false)
File permission : 168
bool(true)
bool(true)
bool(false)
File permission : 169
bool(true)
bool(true)
bool(false)
File permission : 170
bool(true)
bool(true)
bool(false)
File permission : 171
bool(true)
bool(true)
bool(false)
File permission : 172
bool(true)
bool(true)
bool(false)
File permission : 173
bool(true)
bool(true)
bool(false)
File permission : 174
bool(true)
bool(true)
bool(false)
File permission : 175
bool(true)
bool(true)
bool(false)
File permission : 176
bool(true)
bool(true)
bool(false)
File permission : 177
bool(true)
bool(true)
bool(false)
File permission : 178
bool(true)
bool(true)
bool(false)
File permission : 179
bool(true)
bool(true)
bool(false)
File permission : 180
bool(true)
bool(true)
bool(false)
File permission : 181
bool(true)
bool(true)
bool(false)
File permission : 182
bool(true)
bool(true)
bool(false)
File permission : 183
bool(true)
bool(true)
bool(false)
File permission : 184
bool(true)
bool(true)
bool(false)
File permission : 185
bool(true)
bool(true)
bool(false)
File permission : 186
bool(true)
bool(true)
bool(false)
File permission : 187
bool(true)
bool(true)
bool(false)
File permission : 188
bool(true)
bool(true)
bool(false)
File permission : 189
bool(true)
bool(true)
bool(false)
File permission : 190
bool(true)
bool(true)
bool(false)
File permission : 191
bool(true)
bool(true)
bool(false)
File permission : 192
bool(true)
bool(true)
bool(false)
File permission : 193
bool(true)
bool(true)
bool(false)
File permission : 194
bool(true)
bool(true)
bool(false)
File permission : 195
bool(true)
bool(true)
bool(false)
File permission : 196
bool(true)
bool(true)
bool(false)
File permission : 197
bool(true)
bool(true)
bool(false)
File permission : 198
bool(true)
bool(true)
bool(false)
File permission : 199
bool(true)
bool(true)
bool(false)
File permission : 200
bool(true)
bool(true)
bool(false)
File permission : 201
bool(true)
bool(true)
bool(false)
File permission : 202
bool(true)
bool(true)
bool(false)
File permission : 203
bool(true)
bool(true)
bool(false)
File permission : 204
bool(true)
bool(true)
bool(false)
File permission : 205
bool(true)
bool(true)
bool(false)
File permission : 206
bool(true)
bool(true)
bool(false)
File permission : 207
bool(true)
bool(true)
bool(false)
File permission : 208
bool(true)
bool(true)
bool(false)
File permission : 209
bool(true)
bool(true)
bool(false)
File permission : 210
bool(true)
bool(true)
bool(false)
File permission : 211
bool(true)
bool(true)
bool(false)
File permission : 212
bool(true)
bool(true)
bool(false)
File permission : 213
bool(true)
bool(true)
bool(false)
File permission : 214
bool(true)
bool(true)
bool(false)
File permission : 215
bool(true)
bool(true)
bool(false)
File permission : 216
bool(true)
bool(true)
bool(false)
File permission : 217
bool(true)
bool(true)
bool(false)
File permission : 218
bool(true)
bool(true)
bool(false)
File permission : 219
bool(true)
bool(true)
bool(false)
File permission : 220
bool(true)
bool(true)
bool(false)
File permission : 221
bool(true)
bool(true)
bool(false)
File permission : 222
bool(true)
bool(true)
bool(false)
File permission : 223
bool(true)
bool(true)
bool(false)
File permission : 224
bool(true)
bool(true)
bool(false)
File permission : 225
bool(true)
bool(true)
bool(false)
File permission : 226
bool(true)
bool(true)
bool(false)
File permission : 227
bool(true)
bool(true)
bool(false)
File permission : 228
bool(true)
bool(true)
bool(false)
File permission : 229
bool(true)
bool(true)
bool(false)
File permission : 230
bool(true)
bool(true)
bool(false)
File permission : 231
bool(true)
bool(true)
bool(false)
File permission : 232
bool(true)
bool(true)
bool(false)
File permission : 233
bool(true)
bool(true)
bool(false)
File permission : 234
bool(true)
bool(true)
bool(false)
File permission : 235
bool(true)
bool(true)
bool(false)
File permission : 236
bool(true)
bool(true)
bool(false)
File permission : 237
bool(true)
bool(true)
bool(false)
File permission : 238
bool(true)
bool(true)
bool(false)
File permission : 239
bool(true)
bool(true)
bool(false)
File permission : 240
bool(true)
bool(true)
bool(false)
File permission : 241
bool(true)
bool(true)
bool(false)
File permission : 242
bool(true)
bool(true)
bool(false)
File permission : 243
bool(true)
bool(true)
bool(false)
File permission : 244
bool(true)
bool(true)
bool(false)
File permission : 245
bool(true)
bool(true)
bool(false)
File permission : 246
bool(true)
bool(true)
bool(false)
File permission : 247
bool(true)
bool(true)
bool(false)
File permission : 248
bool(true)
bool(true)
bool(false)
File permission : 249
bool(true)
bool(true)
bool(false)
File permission : 250
bool(true)
bool(true)
bool(false)
File permission : 251
bool(true)
bool(true)
bool(false)
File permission : 252
bool(true)
bool(true)
bool(false)
File permission : 253
bool(true)
bool(true)
bool(false)
File permission : 254
bool(true)
bool(true)
bool(false)
File permission : 255
bool(true)
bool(true)
bool(false)
File permission : 256
bool(true)
bool(true)
bool(false)
File permission : 257
bool(true)
bool(true)
bool(false)
File permission : 258
bool(true)
bool(true)
bool(false)
File permission : 259
bool(true)
bool(true)
bool(false)
File permission : 260
bool(true)
bool(true)
bool(false)
File permission : 261
bool(true)
bool(true)
bool(false)
File permission : 262
bool(true)
bool(true)
bool(false)
File permission : 263
bool(true)
bool(true)
bool(false)
File permission : 264
bool(true)
bool(true)
bool(false)
File permission : 265
bool(true)
bool(true)
bool(false)
File permission : 266
bool(true)
bool(true)
bool(false)
File permission : 267
bool(true)
bool(true)
bool(false)
File permission : 268
bool(true)
bool(true)
bool(false)
File permission : 269
bool(true)
bool(true)
bool(false)
File permission : 270
bool(true)
bool(true)
bool(false)
File permission : 271
bool(true)
bool(true)
bool(false)
File permission : 272
bool(true)
bool(true)
bool(false)
File permission : 273
bool(true)
bool(true)
bool(false)
File permission : 274
bool(true)
bool(true)
bool(false)
File permission : 275
bool(true)
bool(true)
bool(false)
File permission : 276
bool(true)
bool(true)
bool(false)
File permission : 277
bool(true)
bool(true)
bool(false)
File permission : 278
bool(true)
bool(true)
bool(false)
File permission : 279
bool(true)
bool(true)
bool(false)
File permission : 280
bool(true)
bool(true)
bool(false)
File permission : 281
bool(true)
bool(true)
bool(false)
File permission : 282
bool(true)
bool(true)
bool(false)
File permission : 283
bool(true)
bool(true)
bool(false)
File permission : 284
bool(true)
bool(true)
bool(false)
File permission : 285
bool(true)
bool(true)
bool(false)
File permission : 286
bool(true)
bool(true)
bool(false)
File permission : 287
bool(true)
bool(true)
bool(false)
File permission : 288
bool(true)
bool(true)
bool(false)
File permission : 289
bool(true)
bool(true)
bool(false)
File permission : 290
bool(true)
bool(true)
bool(false)
File permission : 291
bool(true)
bool(true)
bool(false)
File permission : 292
bool(true)
bool(true)
bool(false)
File permission : 293
bool(true)
bool(true)
bool(false)
File permission : 294
bool(true)
bool(true)
bool(false)
File permission : 295
bool(true)
bool(true)
bool(false)
File permission : 296
bool(true)
bool(true)
bool(false)
File permission : 297
bool(true)
bool(true)
bool(false)
File permission : 298
bool(true)
bool(true)
bool(false)
File permission : 299
bool(true)
bool(true)
bool(false)
File permission : 300
bool(true)
bool(true)
bool(false)
File permission : 301
bool(true)
bool(true)
bool(false)
File permission : 302
bool(true)
bool(true)
bool(false)
File permission : 303
bool(true)
bool(true)
bool(false)
File permission : 304
bool(true)
bool(true)
bool(false)
File permission : 305
bool(true)
bool(true)
bool(false)
File permission : 306
bool(true)
bool(true)
bool(false)
File permission : 307
bool(true)
bool(true)
bool(false)
File permission : 308
bool(true)
bool(true)
bool(false)
File permission : 309
bool(true)
bool(true)
bool(false)
File permission : 310
bool(true)
bool(true)
bool(false)
File permission : 311
bool(true)
bool(true)
bool(false)
File permission : 312
bool(true)
bool(true)
bool(false)
File permission : 313
bool(true)
bool(true)
bool(false)
File permission : 314
bool(true)
bool(true)
bool(false)
File permission : 315
bool(true)
bool(true)
bool(false)
File permission : 316
bool(true)
bool(true)
bool(false)
File permission : 317
bool(true)
bool(true)
bool(false)
File permission : 318
bool(true)
bool(true)
bool(false)
File permission : 319
bool(true)
bool(true)
bool(false)
File permission : 320
bool(true)
bool(true)
bool(false)
File permission : 321
bool(true)
bool(true)
bool(false)
File permission : 322
bool(true)
bool(true)
bool(false)
File permission : 323
bool(true)
bool(true)
bool(false)
File permission : 324
bool(true)
bool(true)
bool(false)
File permission : 325
bool(true)
bool(true)
bool(false)
File permission : 326
bool(true)
bool(true)
bool(false)
File permission : 327
bool(true)
bool(true)
bool(false)
File permission : 328
bool(true)
bool(true)
bool(false)
File permission : 329
bool(true)
bool(true)
bool(false)
File permission : 330
bool(true)
bool(true)
bool(false)
File permission : 331
bool(true)
bool(true)
bool(false)
File permission : 332
bool(true)
bool(true)
bool(false)
File permission : 333
bool(true)
bool(true)
bool(false)
File permission : 334
bool(true)
bool(true)
bool(false)
File permission : 335
bool(true)
bool(true)
bool(false)
File permission : 336
bool(true)
bool(true)
bool(false)
File permission : 337
bool(true)
bool(true)
bool(false)
File permission : 338
bool(true)
bool(true)
bool(false)
File permission : 339
bool(true)
bool(true)
bool(false)
File permission : 340
bool(true)
bool(true)
bool(false)
File permission : 341
bool(true)
bool(true)
bool(false)
File permission : 342
bool(true)
bool(true)
bool(false)
File permission : 343
bool(true)
bool(true)
bool(false)
File permission : 344
bool(true)
bool(true)
bool(false)
File permission : 345
bool(true)
bool(true)
bool(false)
File permission : 346
bool(true)
bool(true)
bool(false)
File permission : 347
bool(true)
bool(true)
bool(false)
File permission : 348
bool(true)
bool(true)
bool(false)
File permission : 349
bool(true)
bool(true)
bool(false)
File permission : 350
bool(true)
bool(true)
bool(false)
File permission : 351
bool(true)
bool(true)
bool(false)
File permission : 352
bool(true)
bool(true)
bool(false)
File permission : 353
bool(true)
bool(true)
bool(false)
File permission : 354
bool(true)
bool(true)
bool(false)
File permission : 355
bool(true)
bool(true)
bool(false)
File permission : 356
bool(true)
bool(true)
bool(false)
File permission : 357
bool(true)
bool(true)
bool(false)
File permission : 358
bool(true)
bool(true)
bool(false)
File permission : 359
bool(true)
bool(true)
bool(false)
File permission : 360
bool(true)
bool(true)
bool(false)
File permission : 361
bool(true)
bool(true)
bool(false)
File permission : 362
bool(true)
bool(true)
bool(false)
File permission : 363
bool(true)
bool(true)
bool(false)
File permission : 364
bool(true)
bool(true)
bool(false)
File permission : 365
bool(true)
bool(true)
bool(false)
File permission : 366
bool(true)
bool(true)
bool(false)
File permission : 367
bool(true)
bool(true)
bool(false)
File permission : 368
bool(true)
bool(true)
bool(false)
File permission : 369
bool(true)
bool(true)
bool(false)
File permission : 370
bool(true)
bool(true)
bool(false)
File permission : 371
bool(true)
bool(true)
bool(false)
File permission : 372
bool(true)
bool(true)
bool(false)
File permission : 373
bool(true)
bool(true)
bool(false)
File permission : 374
bool(true)
bool(true)
bool(false)
File permission : 375
bool(true)
bool(true)
bool(false)
File permission : 376
bool(true)
bool(true)
bool(false)
File permission : 377
bool(true)
bool(true)
bool(false)
File permission : 378
bool(true)
bool(true)
bool(false)
File permission : 379
bool(true)
bool(true)
bool(false)
File permission : 380
bool(true)
bool(true)
bool(false)
File permission : 381
bool(true)
bool(true)
bool(false)
File permission : 382
bool(true)
bool(true)
bool(false)
File permission : 383
bool(true)
bool(true)
bool(false)
File permission : 384
bool(true)
bool(true)
bool(false)
File permission : 385
bool(true)
bool(true)
bool(false)
File permission : 386
bool(true)
bool(true)
bool(false)
File permission : 387
bool(true)
bool(true)
bool(false)
File permission : 388
bool(true)
bool(true)
bool(false)
File permission : 389
bool(true)
bool(true)
bool(false)
File permission : 390
bool(true)
bool(true)
bool(false)
File permission : 391
bool(true)
bool(true)
bool(false)
File permission : 392
bool(true)
bool(true)
bool(false)
File permission : 393
bool(true)
bool(true)
bool(false)
File permission : 394
bool(true)
bool(true)
bool(false)
File permission : 395
bool(true)
bool(true)
bool(false)
File permission : 396
bool(true)
bool(true)
bool(false)
File permission : 397
bool(true)
bool(true)
bool(false)
File permission : 398
bool(true)
bool(true)
bool(false)
File permission : 399
bool(true)
bool(true)
bool(false)
File permission : 400
bool(true)
bool(true)
bool(false)
File permission : 401
bool(true)
bool(true)
bool(false)
File permission : 402
bool(true)
bool(true)
bool(false)
File permission : 403
bool(true)
bool(true)
bool(false)
File permission : 404
bool(true)
bool(true)
bool(false)
File permission : 405
bool(true)
bool(true)
bool(false)
File permission : 406
bool(true)
bool(true)
bool(false)
File permission : 407
bool(true)
bool(true)
bool(false)
File permission : 408
bool(true)
bool(true)
bool(false)
File permission : 409
bool(true)
bool(true)
bool(false)
File permission : 410
bool(true)
bool(true)
bool(false)
File permission : 411
bool(true)
bool(true)
bool(false)
File permission : 412
bool(true)
bool(true)
bool(false)
File permission : 413
bool(true)
bool(true)
bool(false)
File permission : 414
bool(true)
bool(true)
bool(false)
File permission : 415
bool(true)
bool(true)
bool(false)
File permission : 416
bool(true)
bool(true)
bool(false)
File permission : 417
bool(true)
bool(true)
bool(false)
File permission : 418
bool(true)
bool(true)
bool(false)
File permission : 419
bool(true)
bool(true)
bool(false)
File permission : 420
bool(true)
bool(true)
bool(false)
File permission : 421
bool(true)
bool(true)
bool(false)
File permission : 422
bool(true)
bool(true)
bool(false)
File permission : 423
bool(true)
bool(true)
bool(false)
File permission : 424
bool(true)
bool(true)
bool(false)
File permission : 425
bool(true)
bool(true)
bool(false)
File permission : 426
bool(true)
bool(true)
bool(false)
File permission : 427
bool(true)
bool(true)
bool(false)
File permission : 428
bool(true)
bool(true)
bool(false)
File permission : 429
bool(true)
bool(true)
bool(false)
File permission : 430
bool(true)
bool(true)
bool(false)
File permission : 431
bool(true)
bool(true)
bool(false)
File permission : 432
bool(true)
bool(true)
bool(false)
File permission : 433
bool(true)
bool(true)
bool(false)
File permission : 434
bool(true)
bool(true)
bool(false)
File permission : 435
bool(true)
bool(true)
bool(false)
File permission : 436
bool(true)
bool(true)
bool(false)
File permission : 437
bool(true)
bool(true)
bool(false)
File permission : 438
bool(true)
bool(true)
bool(false)
File permission : 439
bool(true)
bool(true)
bool(false)
File permission : 440
bool(true)
bool(true)
bool(false)
File permission : 441
bool(true)
bool(true)
bool(false)
File permission : 442
bool(true)
bool(true)
bool(false)
File permission : 443
bool(true)
bool(true)
bool(false)
File permission : 444
bool(true)
bool(true)
bool(false)
File permission : 445
bool(true)
bool(true)
bool(false)
File permission : 446
bool(true)
bool(true)
bool(false)
File permission : 447
bool(true)
bool(true)
bool(false)
File permission : 448
bool(true)
bool(true)
bool(false)
File permission : 449
bool(true)
bool(true)
bool(false)
File permission : 450
bool(true)
bool(true)
bool(false)
File permission : 451
bool(true)
bool(true)
bool(false)
File permission : 452
bool(true)
bool(true)
bool(false)
File permission : 453
bool(true)
bool(true)
bool(false)
File permission : 454
bool(true)
bool(true)
bool(false)
File permission : 455
bool(true)
bool(true)
bool(false)
File permission : 456
bool(true)
bool(true)
bool(false)
File permission : 457
bool(true)
bool(true)
bool(false)
File permission : 458
bool(true)
bool(true)
bool(false)
File permission : 459
bool(true)
bool(true)
bool(false)
File permission : 460
bool(true)
bool(true)
bool(false)
File permission : 461
bool(true)
bool(true)
bool(false)
File permission : 462
bool(true)
bool(true)
bool(false)
File permission : 463
bool(true)
bool(true)
bool(false)
File permission : 464
bool(true)
bool(true)
bool(false)
File permission : 465
bool(true)
bool(true)
bool(false)
File permission : 466
bool(true)
bool(true)
bool(false)
File permission : 467
bool(true)
bool(true)
bool(false)
File permission : 468
bool(true)
bool(true)
bool(false)
File permission : 469
bool(true)
bool(true)
bool(false)
File permission : 470
bool(true)
bool(true)
bool(false)
File permission : 471
bool(true)
bool(true)
bool(false)
File permission : 472
bool(true)
bool(true)
bool(false)
File permission : 473
bool(true)
bool(true)
bool(false)
File permission : 474
bool(true)
bool(true)
bool(false)
File permission : 475
bool(true)
bool(true)
bool(false)
File permission : 476
bool(true)
bool(true)
bool(false)
File permission : 477
bool(true)
bool(true)
bool(false)
File permission : 478
bool(true)
bool(true)
bool(false)
File permission : 479
bool(true)
bool(true)
bool(false)
File permission : 480
bool(true)
bool(true)
bool(false)
File permission : 481
bool(true)
bool(true)
bool(false)
File permission : 482
bool(true)
bool(true)
bool(false)
File permission : 483
bool(true)
bool(true)
bool(false)
File permission : 484
bool(true)
bool(true)
bool(false)
File permission : 485
bool(true)
bool(true)
bool(false)
File permission : 486
bool(true)
bool(true)
bool(false)
File permission : 487
bool(true)
bool(true)
bool(false)
File permission : 488
bool(true)
bool(true)
bool(false)
File permission : 489
bool(true)
bool(true)
bool(false)
File permission : 490
bool(true)
bool(true)
bool(false)
File permission : 491
bool(true)
bool(true)
bool(false)
File permission : 492
bool(true)
bool(true)
bool(false)
File permission : 493
bool(true)
bool(true)
bool(false)
File permission : 494
bool(true)
bool(true)
bool(false)
File permission : 495
bool(true)
bool(true)
bool(false)
File permission : 496
bool(true)
bool(true)
bool(false)
File permission : 497
bool(true)
bool(true)
bool(false)
File permission : 498
bool(true)
bool(true)
bool(false)
File permission : 499
bool(true)
bool(true)
bool(false)
File permission : 500
bool(true)
bool(true)
bool(false)
File permission : 501
bool(true)
bool(true)
bool(false)
File permission : 502
bool(true)
bool(true)
bool(false)
File permission : 503
bool(true)
bool(true)
bool(false)
File permission : 504
bool(true)
bool(true)
bool(false)
File permission : 505
bool(true)
bool(true)
bool(false)
File permission : 506
bool(true)
bool(true)
bool(false)
File permission : 507
bool(true)
bool(true)
bool(false)
File permission : 508
bool(true)
bool(true)
bool(false)
File permission : 509
bool(true)
bool(true)
bool(false)
File permission : 510
bool(true)
bool(true)
bool(false)
File permission : 511
bool(true)
bool(true)
bool(false)
Done
