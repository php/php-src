--TEST--
Test fileperms() & chmod() functions: usage variation - perms(0000-0777)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not on Windows');
}
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
/*
  Prototype: int fileperms ( string $filename );
  Description: Returns the permissions on the file, or FALSE in case of an error

  Prototype: bool chmod ( string $filename, int $mode );
  Description: Attempts to change the mode of the file specified by
               filename to that given in mode
*/

echo "*** Testing fileperms() & chmod() : usage variations ***\n";

$file_name = __DIR__."/006_variation1.tmp";
$file_handle = fopen($file_name, "w");
fclose($file_handle);
$dir_name = __DIR__."/006_variation1";
mkdir($dir_name);

$count = 1;
echo "-- Testing all permission from octal 0000 to octal 0777 on file and dir --\n";
for($mode = 0000; $mode <= 0777; $mode++) {
  echo "-- Iteration $count --\n";
  var_dump( chmod($file_name, $mode) );
  printf("%o", fileperms($file_name) );
  echo "\n";
  clearstatcache();

  var_dump( chmod($dir_name, $mode) );
  printf("%o", fileperms($dir_name) );
  echo "\n";
  clearstatcache();
  $count++;
}

echo "*** Done ***\n";
?>
--CLEAN--
<?php
chmod(__DIR__."/006_variation1.tmp", 0777);
chmod(__DIR__."/006_variation1", 0777);
unlink(__DIR__."/006_variation1.tmp");
rmdir(__DIR__."/006_variation1");
?>
--EXPECT--
*** Testing fileperms() & chmod() : usage variations ***
-- Testing all permission from octal 0000 to octal 0777 on file and dir --
-- Iteration 1 --
bool(true)
100000
bool(true)
40000
-- Iteration 2 --
bool(true)
100001
bool(true)
40001
-- Iteration 3 --
bool(true)
100002
bool(true)
40002
-- Iteration 4 --
bool(true)
100003
bool(true)
40003
-- Iteration 5 --
bool(true)
100004
bool(true)
40004
-- Iteration 6 --
bool(true)
100005
bool(true)
40005
-- Iteration 7 --
bool(true)
100006
bool(true)
40006
-- Iteration 8 --
bool(true)
100007
bool(true)
40007
-- Iteration 9 --
bool(true)
100010
bool(true)
40010
-- Iteration 10 --
bool(true)
100011
bool(true)
40011
-- Iteration 11 --
bool(true)
100012
bool(true)
40012
-- Iteration 12 --
bool(true)
100013
bool(true)
40013
-- Iteration 13 --
bool(true)
100014
bool(true)
40014
-- Iteration 14 --
bool(true)
100015
bool(true)
40015
-- Iteration 15 --
bool(true)
100016
bool(true)
40016
-- Iteration 16 --
bool(true)
100017
bool(true)
40017
-- Iteration 17 --
bool(true)
100020
bool(true)
40020
-- Iteration 18 --
bool(true)
100021
bool(true)
40021
-- Iteration 19 --
bool(true)
100022
bool(true)
40022
-- Iteration 20 --
bool(true)
100023
bool(true)
40023
-- Iteration 21 --
bool(true)
100024
bool(true)
40024
-- Iteration 22 --
bool(true)
100025
bool(true)
40025
-- Iteration 23 --
bool(true)
100026
bool(true)
40026
-- Iteration 24 --
bool(true)
100027
bool(true)
40027
-- Iteration 25 --
bool(true)
100030
bool(true)
40030
-- Iteration 26 --
bool(true)
100031
bool(true)
40031
-- Iteration 27 --
bool(true)
100032
bool(true)
40032
-- Iteration 28 --
bool(true)
100033
bool(true)
40033
-- Iteration 29 --
bool(true)
100034
bool(true)
40034
-- Iteration 30 --
bool(true)
100035
bool(true)
40035
-- Iteration 31 --
bool(true)
100036
bool(true)
40036
-- Iteration 32 --
bool(true)
100037
bool(true)
40037
-- Iteration 33 --
bool(true)
100040
bool(true)
40040
-- Iteration 34 --
bool(true)
100041
bool(true)
40041
-- Iteration 35 --
bool(true)
100042
bool(true)
40042
-- Iteration 36 --
bool(true)
100043
bool(true)
40043
-- Iteration 37 --
bool(true)
100044
bool(true)
40044
-- Iteration 38 --
bool(true)
100045
bool(true)
40045
-- Iteration 39 --
bool(true)
100046
bool(true)
40046
-- Iteration 40 --
bool(true)
100047
bool(true)
40047
-- Iteration 41 --
bool(true)
100050
bool(true)
40050
-- Iteration 42 --
bool(true)
100051
bool(true)
40051
-- Iteration 43 --
bool(true)
100052
bool(true)
40052
-- Iteration 44 --
bool(true)
100053
bool(true)
40053
-- Iteration 45 --
bool(true)
100054
bool(true)
40054
-- Iteration 46 --
bool(true)
100055
bool(true)
40055
-- Iteration 47 --
bool(true)
100056
bool(true)
40056
-- Iteration 48 --
bool(true)
100057
bool(true)
40057
-- Iteration 49 --
bool(true)
100060
bool(true)
40060
-- Iteration 50 --
bool(true)
100061
bool(true)
40061
-- Iteration 51 --
bool(true)
100062
bool(true)
40062
-- Iteration 52 --
bool(true)
100063
bool(true)
40063
-- Iteration 53 --
bool(true)
100064
bool(true)
40064
-- Iteration 54 --
bool(true)
100065
bool(true)
40065
-- Iteration 55 --
bool(true)
100066
bool(true)
40066
-- Iteration 56 --
bool(true)
100067
bool(true)
40067
-- Iteration 57 --
bool(true)
100070
bool(true)
40070
-- Iteration 58 --
bool(true)
100071
bool(true)
40071
-- Iteration 59 --
bool(true)
100072
bool(true)
40072
-- Iteration 60 --
bool(true)
100073
bool(true)
40073
-- Iteration 61 --
bool(true)
100074
bool(true)
40074
-- Iteration 62 --
bool(true)
100075
bool(true)
40075
-- Iteration 63 --
bool(true)
100076
bool(true)
40076
-- Iteration 64 --
bool(true)
100077
bool(true)
40077
-- Iteration 65 --
bool(true)
100100
bool(true)
40100
-- Iteration 66 --
bool(true)
100101
bool(true)
40101
-- Iteration 67 --
bool(true)
100102
bool(true)
40102
-- Iteration 68 --
bool(true)
100103
bool(true)
40103
-- Iteration 69 --
bool(true)
100104
bool(true)
40104
-- Iteration 70 --
bool(true)
100105
bool(true)
40105
-- Iteration 71 --
bool(true)
100106
bool(true)
40106
-- Iteration 72 --
bool(true)
100107
bool(true)
40107
-- Iteration 73 --
bool(true)
100110
bool(true)
40110
-- Iteration 74 --
bool(true)
100111
bool(true)
40111
-- Iteration 75 --
bool(true)
100112
bool(true)
40112
-- Iteration 76 --
bool(true)
100113
bool(true)
40113
-- Iteration 77 --
bool(true)
100114
bool(true)
40114
-- Iteration 78 --
bool(true)
100115
bool(true)
40115
-- Iteration 79 --
bool(true)
100116
bool(true)
40116
-- Iteration 80 --
bool(true)
100117
bool(true)
40117
-- Iteration 81 --
bool(true)
100120
bool(true)
40120
-- Iteration 82 --
bool(true)
100121
bool(true)
40121
-- Iteration 83 --
bool(true)
100122
bool(true)
40122
-- Iteration 84 --
bool(true)
100123
bool(true)
40123
-- Iteration 85 --
bool(true)
100124
bool(true)
40124
-- Iteration 86 --
bool(true)
100125
bool(true)
40125
-- Iteration 87 --
bool(true)
100126
bool(true)
40126
-- Iteration 88 --
bool(true)
100127
bool(true)
40127
-- Iteration 89 --
bool(true)
100130
bool(true)
40130
-- Iteration 90 --
bool(true)
100131
bool(true)
40131
-- Iteration 91 --
bool(true)
100132
bool(true)
40132
-- Iteration 92 --
bool(true)
100133
bool(true)
40133
-- Iteration 93 --
bool(true)
100134
bool(true)
40134
-- Iteration 94 --
bool(true)
100135
bool(true)
40135
-- Iteration 95 --
bool(true)
100136
bool(true)
40136
-- Iteration 96 --
bool(true)
100137
bool(true)
40137
-- Iteration 97 --
bool(true)
100140
bool(true)
40140
-- Iteration 98 --
bool(true)
100141
bool(true)
40141
-- Iteration 99 --
bool(true)
100142
bool(true)
40142
-- Iteration 100 --
bool(true)
100143
bool(true)
40143
-- Iteration 101 --
bool(true)
100144
bool(true)
40144
-- Iteration 102 --
bool(true)
100145
bool(true)
40145
-- Iteration 103 --
bool(true)
100146
bool(true)
40146
-- Iteration 104 --
bool(true)
100147
bool(true)
40147
-- Iteration 105 --
bool(true)
100150
bool(true)
40150
-- Iteration 106 --
bool(true)
100151
bool(true)
40151
-- Iteration 107 --
bool(true)
100152
bool(true)
40152
-- Iteration 108 --
bool(true)
100153
bool(true)
40153
-- Iteration 109 --
bool(true)
100154
bool(true)
40154
-- Iteration 110 --
bool(true)
100155
bool(true)
40155
-- Iteration 111 --
bool(true)
100156
bool(true)
40156
-- Iteration 112 --
bool(true)
100157
bool(true)
40157
-- Iteration 113 --
bool(true)
100160
bool(true)
40160
-- Iteration 114 --
bool(true)
100161
bool(true)
40161
-- Iteration 115 --
bool(true)
100162
bool(true)
40162
-- Iteration 116 --
bool(true)
100163
bool(true)
40163
-- Iteration 117 --
bool(true)
100164
bool(true)
40164
-- Iteration 118 --
bool(true)
100165
bool(true)
40165
-- Iteration 119 --
bool(true)
100166
bool(true)
40166
-- Iteration 120 --
bool(true)
100167
bool(true)
40167
-- Iteration 121 --
bool(true)
100170
bool(true)
40170
-- Iteration 122 --
bool(true)
100171
bool(true)
40171
-- Iteration 123 --
bool(true)
100172
bool(true)
40172
-- Iteration 124 --
bool(true)
100173
bool(true)
40173
-- Iteration 125 --
bool(true)
100174
bool(true)
40174
-- Iteration 126 --
bool(true)
100175
bool(true)
40175
-- Iteration 127 --
bool(true)
100176
bool(true)
40176
-- Iteration 128 --
bool(true)
100177
bool(true)
40177
-- Iteration 129 --
bool(true)
100200
bool(true)
40200
-- Iteration 130 --
bool(true)
100201
bool(true)
40201
-- Iteration 131 --
bool(true)
100202
bool(true)
40202
-- Iteration 132 --
bool(true)
100203
bool(true)
40203
-- Iteration 133 --
bool(true)
100204
bool(true)
40204
-- Iteration 134 --
bool(true)
100205
bool(true)
40205
-- Iteration 135 --
bool(true)
100206
bool(true)
40206
-- Iteration 136 --
bool(true)
100207
bool(true)
40207
-- Iteration 137 --
bool(true)
100210
bool(true)
40210
-- Iteration 138 --
bool(true)
100211
bool(true)
40211
-- Iteration 139 --
bool(true)
100212
bool(true)
40212
-- Iteration 140 --
bool(true)
100213
bool(true)
40213
-- Iteration 141 --
bool(true)
100214
bool(true)
40214
-- Iteration 142 --
bool(true)
100215
bool(true)
40215
-- Iteration 143 --
bool(true)
100216
bool(true)
40216
-- Iteration 144 --
bool(true)
100217
bool(true)
40217
-- Iteration 145 --
bool(true)
100220
bool(true)
40220
-- Iteration 146 --
bool(true)
100221
bool(true)
40221
-- Iteration 147 --
bool(true)
100222
bool(true)
40222
-- Iteration 148 --
bool(true)
100223
bool(true)
40223
-- Iteration 149 --
bool(true)
100224
bool(true)
40224
-- Iteration 150 --
bool(true)
100225
bool(true)
40225
-- Iteration 151 --
bool(true)
100226
bool(true)
40226
-- Iteration 152 --
bool(true)
100227
bool(true)
40227
-- Iteration 153 --
bool(true)
100230
bool(true)
40230
-- Iteration 154 --
bool(true)
100231
bool(true)
40231
-- Iteration 155 --
bool(true)
100232
bool(true)
40232
-- Iteration 156 --
bool(true)
100233
bool(true)
40233
-- Iteration 157 --
bool(true)
100234
bool(true)
40234
-- Iteration 158 --
bool(true)
100235
bool(true)
40235
-- Iteration 159 --
bool(true)
100236
bool(true)
40236
-- Iteration 160 --
bool(true)
100237
bool(true)
40237
-- Iteration 161 --
bool(true)
100240
bool(true)
40240
-- Iteration 162 --
bool(true)
100241
bool(true)
40241
-- Iteration 163 --
bool(true)
100242
bool(true)
40242
-- Iteration 164 --
bool(true)
100243
bool(true)
40243
-- Iteration 165 --
bool(true)
100244
bool(true)
40244
-- Iteration 166 --
bool(true)
100245
bool(true)
40245
-- Iteration 167 --
bool(true)
100246
bool(true)
40246
-- Iteration 168 --
bool(true)
100247
bool(true)
40247
-- Iteration 169 --
bool(true)
100250
bool(true)
40250
-- Iteration 170 --
bool(true)
100251
bool(true)
40251
-- Iteration 171 --
bool(true)
100252
bool(true)
40252
-- Iteration 172 --
bool(true)
100253
bool(true)
40253
-- Iteration 173 --
bool(true)
100254
bool(true)
40254
-- Iteration 174 --
bool(true)
100255
bool(true)
40255
-- Iteration 175 --
bool(true)
100256
bool(true)
40256
-- Iteration 176 --
bool(true)
100257
bool(true)
40257
-- Iteration 177 --
bool(true)
100260
bool(true)
40260
-- Iteration 178 --
bool(true)
100261
bool(true)
40261
-- Iteration 179 --
bool(true)
100262
bool(true)
40262
-- Iteration 180 --
bool(true)
100263
bool(true)
40263
-- Iteration 181 --
bool(true)
100264
bool(true)
40264
-- Iteration 182 --
bool(true)
100265
bool(true)
40265
-- Iteration 183 --
bool(true)
100266
bool(true)
40266
-- Iteration 184 --
bool(true)
100267
bool(true)
40267
-- Iteration 185 --
bool(true)
100270
bool(true)
40270
-- Iteration 186 --
bool(true)
100271
bool(true)
40271
-- Iteration 187 --
bool(true)
100272
bool(true)
40272
-- Iteration 188 --
bool(true)
100273
bool(true)
40273
-- Iteration 189 --
bool(true)
100274
bool(true)
40274
-- Iteration 190 --
bool(true)
100275
bool(true)
40275
-- Iteration 191 --
bool(true)
100276
bool(true)
40276
-- Iteration 192 --
bool(true)
100277
bool(true)
40277
-- Iteration 193 --
bool(true)
100300
bool(true)
40300
-- Iteration 194 --
bool(true)
100301
bool(true)
40301
-- Iteration 195 --
bool(true)
100302
bool(true)
40302
-- Iteration 196 --
bool(true)
100303
bool(true)
40303
-- Iteration 197 --
bool(true)
100304
bool(true)
40304
-- Iteration 198 --
bool(true)
100305
bool(true)
40305
-- Iteration 199 --
bool(true)
100306
bool(true)
40306
-- Iteration 200 --
bool(true)
100307
bool(true)
40307
-- Iteration 201 --
bool(true)
100310
bool(true)
40310
-- Iteration 202 --
bool(true)
100311
bool(true)
40311
-- Iteration 203 --
bool(true)
100312
bool(true)
40312
-- Iteration 204 --
bool(true)
100313
bool(true)
40313
-- Iteration 205 --
bool(true)
100314
bool(true)
40314
-- Iteration 206 --
bool(true)
100315
bool(true)
40315
-- Iteration 207 --
bool(true)
100316
bool(true)
40316
-- Iteration 208 --
bool(true)
100317
bool(true)
40317
-- Iteration 209 --
bool(true)
100320
bool(true)
40320
-- Iteration 210 --
bool(true)
100321
bool(true)
40321
-- Iteration 211 --
bool(true)
100322
bool(true)
40322
-- Iteration 212 --
bool(true)
100323
bool(true)
40323
-- Iteration 213 --
bool(true)
100324
bool(true)
40324
-- Iteration 214 --
bool(true)
100325
bool(true)
40325
-- Iteration 215 --
bool(true)
100326
bool(true)
40326
-- Iteration 216 --
bool(true)
100327
bool(true)
40327
-- Iteration 217 --
bool(true)
100330
bool(true)
40330
-- Iteration 218 --
bool(true)
100331
bool(true)
40331
-- Iteration 219 --
bool(true)
100332
bool(true)
40332
-- Iteration 220 --
bool(true)
100333
bool(true)
40333
-- Iteration 221 --
bool(true)
100334
bool(true)
40334
-- Iteration 222 --
bool(true)
100335
bool(true)
40335
-- Iteration 223 --
bool(true)
100336
bool(true)
40336
-- Iteration 224 --
bool(true)
100337
bool(true)
40337
-- Iteration 225 --
bool(true)
100340
bool(true)
40340
-- Iteration 226 --
bool(true)
100341
bool(true)
40341
-- Iteration 227 --
bool(true)
100342
bool(true)
40342
-- Iteration 228 --
bool(true)
100343
bool(true)
40343
-- Iteration 229 --
bool(true)
100344
bool(true)
40344
-- Iteration 230 --
bool(true)
100345
bool(true)
40345
-- Iteration 231 --
bool(true)
100346
bool(true)
40346
-- Iteration 232 --
bool(true)
100347
bool(true)
40347
-- Iteration 233 --
bool(true)
100350
bool(true)
40350
-- Iteration 234 --
bool(true)
100351
bool(true)
40351
-- Iteration 235 --
bool(true)
100352
bool(true)
40352
-- Iteration 236 --
bool(true)
100353
bool(true)
40353
-- Iteration 237 --
bool(true)
100354
bool(true)
40354
-- Iteration 238 --
bool(true)
100355
bool(true)
40355
-- Iteration 239 --
bool(true)
100356
bool(true)
40356
-- Iteration 240 --
bool(true)
100357
bool(true)
40357
-- Iteration 241 --
bool(true)
100360
bool(true)
40360
-- Iteration 242 --
bool(true)
100361
bool(true)
40361
-- Iteration 243 --
bool(true)
100362
bool(true)
40362
-- Iteration 244 --
bool(true)
100363
bool(true)
40363
-- Iteration 245 --
bool(true)
100364
bool(true)
40364
-- Iteration 246 --
bool(true)
100365
bool(true)
40365
-- Iteration 247 --
bool(true)
100366
bool(true)
40366
-- Iteration 248 --
bool(true)
100367
bool(true)
40367
-- Iteration 249 --
bool(true)
100370
bool(true)
40370
-- Iteration 250 --
bool(true)
100371
bool(true)
40371
-- Iteration 251 --
bool(true)
100372
bool(true)
40372
-- Iteration 252 --
bool(true)
100373
bool(true)
40373
-- Iteration 253 --
bool(true)
100374
bool(true)
40374
-- Iteration 254 --
bool(true)
100375
bool(true)
40375
-- Iteration 255 --
bool(true)
100376
bool(true)
40376
-- Iteration 256 --
bool(true)
100377
bool(true)
40377
-- Iteration 257 --
bool(true)
100400
bool(true)
40400
-- Iteration 258 --
bool(true)
100401
bool(true)
40401
-- Iteration 259 --
bool(true)
100402
bool(true)
40402
-- Iteration 260 --
bool(true)
100403
bool(true)
40403
-- Iteration 261 --
bool(true)
100404
bool(true)
40404
-- Iteration 262 --
bool(true)
100405
bool(true)
40405
-- Iteration 263 --
bool(true)
100406
bool(true)
40406
-- Iteration 264 --
bool(true)
100407
bool(true)
40407
-- Iteration 265 --
bool(true)
100410
bool(true)
40410
-- Iteration 266 --
bool(true)
100411
bool(true)
40411
-- Iteration 267 --
bool(true)
100412
bool(true)
40412
-- Iteration 268 --
bool(true)
100413
bool(true)
40413
-- Iteration 269 --
bool(true)
100414
bool(true)
40414
-- Iteration 270 --
bool(true)
100415
bool(true)
40415
-- Iteration 271 --
bool(true)
100416
bool(true)
40416
-- Iteration 272 --
bool(true)
100417
bool(true)
40417
-- Iteration 273 --
bool(true)
100420
bool(true)
40420
-- Iteration 274 --
bool(true)
100421
bool(true)
40421
-- Iteration 275 --
bool(true)
100422
bool(true)
40422
-- Iteration 276 --
bool(true)
100423
bool(true)
40423
-- Iteration 277 --
bool(true)
100424
bool(true)
40424
-- Iteration 278 --
bool(true)
100425
bool(true)
40425
-- Iteration 279 --
bool(true)
100426
bool(true)
40426
-- Iteration 280 --
bool(true)
100427
bool(true)
40427
-- Iteration 281 --
bool(true)
100430
bool(true)
40430
-- Iteration 282 --
bool(true)
100431
bool(true)
40431
-- Iteration 283 --
bool(true)
100432
bool(true)
40432
-- Iteration 284 --
bool(true)
100433
bool(true)
40433
-- Iteration 285 --
bool(true)
100434
bool(true)
40434
-- Iteration 286 --
bool(true)
100435
bool(true)
40435
-- Iteration 287 --
bool(true)
100436
bool(true)
40436
-- Iteration 288 --
bool(true)
100437
bool(true)
40437
-- Iteration 289 --
bool(true)
100440
bool(true)
40440
-- Iteration 290 --
bool(true)
100441
bool(true)
40441
-- Iteration 291 --
bool(true)
100442
bool(true)
40442
-- Iteration 292 --
bool(true)
100443
bool(true)
40443
-- Iteration 293 --
bool(true)
100444
bool(true)
40444
-- Iteration 294 --
bool(true)
100445
bool(true)
40445
-- Iteration 295 --
bool(true)
100446
bool(true)
40446
-- Iteration 296 --
bool(true)
100447
bool(true)
40447
-- Iteration 297 --
bool(true)
100450
bool(true)
40450
-- Iteration 298 --
bool(true)
100451
bool(true)
40451
-- Iteration 299 --
bool(true)
100452
bool(true)
40452
-- Iteration 300 --
bool(true)
100453
bool(true)
40453
-- Iteration 301 --
bool(true)
100454
bool(true)
40454
-- Iteration 302 --
bool(true)
100455
bool(true)
40455
-- Iteration 303 --
bool(true)
100456
bool(true)
40456
-- Iteration 304 --
bool(true)
100457
bool(true)
40457
-- Iteration 305 --
bool(true)
100460
bool(true)
40460
-- Iteration 306 --
bool(true)
100461
bool(true)
40461
-- Iteration 307 --
bool(true)
100462
bool(true)
40462
-- Iteration 308 --
bool(true)
100463
bool(true)
40463
-- Iteration 309 --
bool(true)
100464
bool(true)
40464
-- Iteration 310 --
bool(true)
100465
bool(true)
40465
-- Iteration 311 --
bool(true)
100466
bool(true)
40466
-- Iteration 312 --
bool(true)
100467
bool(true)
40467
-- Iteration 313 --
bool(true)
100470
bool(true)
40470
-- Iteration 314 --
bool(true)
100471
bool(true)
40471
-- Iteration 315 --
bool(true)
100472
bool(true)
40472
-- Iteration 316 --
bool(true)
100473
bool(true)
40473
-- Iteration 317 --
bool(true)
100474
bool(true)
40474
-- Iteration 318 --
bool(true)
100475
bool(true)
40475
-- Iteration 319 --
bool(true)
100476
bool(true)
40476
-- Iteration 320 --
bool(true)
100477
bool(true)
40477
-- Iteration 321 --
bool(true)
100500
bool(true)
40500
-- Iteration 322 --
bool(true)
100501
bool(true)
40501
-- Iteration 323 --
bool(true)
100502
bool(true)
40502
-- Iteration 324 --
bool(true)
100503
bool(true)
40503
-- Iteration 325 --
bool(true)
100504
bool(true)
40504
-- Iteration 326 --
bool(true)
100505
bool(true)
40505
-- Iteration 327 --
bool(true)
100506
bool(true)
40506
-- Iteration 328 --
bool(true)
100507
bool(true)
40507
-- Iteration 329 --
bool(true)
100510
bool(true)
40510
-- Iteration 330 --
bool(true)
100511
bool(true)
40511
-- Iteration 331 --
bool(true)
100512
bool(true)
40512
-- Iteration 332 --
bool(true)
100513
bool(true)
40513
-- Iteration 333 --
bool(true)
100514
bool(true)
40514
-- Iteration 334 --
bool(true)
100515
bool(true)
40515
-- Iteration 335 --
bool(true)
100516
bool(true)
40516
-- Iteration 336 --
bool(true)
100517
bool(true)
40517
-- Iteration 337 --
bool(true)
100520
bool(true)
40520
-- Iteration 338 --
bool(true)
100521
bool(true)
40521
-- Iteration 339 --
bool(true)
100522
bool(true)
40522
-- Iteration 340 --
bool(true)
100523
bool(true)
40523
-- Iteration 341 --
bool(true)
100524
bool(true)
40524
-- Iteration 342 --
bool(true)
100525
bool(true)
40525
-- Iteration 343 --
bool(true)
100526
bool(true)
40526
-- Iteration 344 --
bool(true)
100527
bool(true)
40527
-- Iteration 345 --
bool(true)
100530
bool(true)
40530
-- Iteration 346 --
bool(true)
100531
bool(true)
40531
-- Iteration 347 --
bool(true)
100532
bool(true)
40532
-- Iteration 348 --
bool(true)
100533
bool(true)
40533
-- Iteration 349 --
bool(true)
100534
bool(true)
40534
-- Iteration 350 --
bool(true)
100535
bool(true)
40535
-- Iteration 351 --
bool(true)
100536
bool(true)
40536
-- Iteration 352 --
bool(true)
100537
bool(true)
40537
-- Iteration 353 --
bool(true)
100540
bool(true)
40540
-- Iteration 354 --
bool(true)
100541
bool(true)
40541
-- Iteration 355 --
bool(true)
100542
bool(true)
40542
-- Iteration 356 --
bool(true)
100543
bool(true)
40543
-- Iteration 357 --
bool(true)
100544
bool(true)
40544
-- Iteration 358 --
bool(true)
100545
bool(true)
40545
-- Iteration 359 --
bool(true)
100546
bool(true)
40546
-- Iteration 360 --
bool(true)
100547
bool(true)
40547
-- Iteration 361 --
bool(true)
100550
bool(true)
40550
-- Iteration 362 --
bool(true)
100551
bool(true)
40551
-- Iteration 363 --
bool(true)
100552
bool(true)
40552
-- Iteration 364 --
bool(true)
100553
bool(true)
40553
-- Iteration 365 --
bool(true)
100554
bool(true)
40554
-- Iteration 366 --
bool(true)
100555
bool(true)
40555
-- Iteration 367 --
bool(true)
100556
bool(true)
40556
-- Iteration 368 --
bool(true)
100557
bool(true)
40557
-- Iteration 369 --
bool(true)
100560
bool(true)
40560
-- Iteration 370 --
bool(true)
100561
bool(true)
40561
-- Iteration 371 --
bool(true)
100562
bool(true)
40562
-- Iteration 372 --
bool(true)
100563
bool(true)
40563
-- Iteration 373 --
bool(true)
100564
bool(true)
40564
-- Iteration 374 --
bool(true)
100565
bool(true)
40565
-- Iteration 375 --
bool(true)
100566
bool(true)
40566
-- Iteration 376 --
bool(true)
100567
bool(true)
40567
-- Iteration 377 --
bool(true)
100570
bool(true)
40570
-- Iteration 378 --
bool(true)
100571
bool(true)
40571
-- Iteration 379 --
bool(true)
100572
bool(true)
40572
-- Iteration 380 --
bool(true)
100573
bool(true)
40573
-- Iteration 381 --
bool(true)
100574
bool(true)
40574
-- Iteration 382 --
bool(true)
100575
bool(true)
40575
-- Iteration 383 --
bool(true)
100576
bool(true)
40576
-- Iteration 384 --
bool(true)
100577
bool(true)
40577
-- Iteration 385 --
bool(true)
100600
bool(true)
40600
-- Iteration 386 --
bool(true)
100601
bool(true)
40601
-- Iteration 387 --
bool(true)
100602
bool(true)
40602
-- Iteration 388 --
bool(true)
100603
bool(true)
40603
-- Iteration 389 --
bool(true)
100604
bool(true)
40604
-- Iteration 390 --
bool(true)
100605
bool(true)
40605
-- Iteration 391 --
bool(true)
100606
bool(true)
40606
-- Iteration 392 --
bool(true)
100607
bool(true)
40607
-- Iteration 393 --
bool(true)
100610
bool(true)
40610
-- Iteration 394 --
bool(true)
100611
bool(true)
40611
-- Iteration 395 --
bool(true)
100612
bool(true)
40612
-- Iteration 396 --
bool(true)
100613
bool(true)
40613
-- Iteration 397 --
bool(true)
100614
bool(true)
40614
-- Iteration 398 --
bool(true)
100615
bool(true)
40615
-- Iteration 399 --
bool(true)
100616
bool(true)
40616
-- Iteration 400 --
bool(true)
100617
bool(true)
40617
-- Iteration 401 --
bool(true)
100620
bool(true)
40620
-- Iteration 402 --
bool(true)
100621
bool(true)
40621
-- Iteration 403 --
bool(true)
100622
bool(true)
40622
-- Iteration 404 --
bool(true)
100623
bool(true)
40623
-- Iteration 405 --
bool(true)
100624
bool(true)
40624
-- Iteration 406 --
bool(true)
100625
bool(true)
40625
-- Iteration 407 --
bool(true)
100626
bool(true)
40626
-- Iteration 408 --
bool(true)
100627
bool(true)
40627
-- Iteration 409 --
bool(true)
100630
bool(true)
40630
-- Iteration 410 --
bool(true)
100631
bool(true)
40631
-- Iteration 411 --
bool(true)
100632
bool(true)
40632
-- Iteration 412 --
bool(true)
100633
bool(true)
40633
-- Iteration 413 --
bool(true)
100634
bool(true)
40634
-- Iteration 414 --
bool(true)
100635
bool(true)
40635
-- Iteration 415 --
bool(true)
100636
bool(true)
40636
-- Iteration 416 --
bool(true)
100637
bool(true)
40637
-- Iteration 417 --
bool(true)
100640
bool(true)
40640
-- Iteration 418 --
bool(true)
100641
bool(true)
40641
-- Iteration 419 --
bool(true)
100642
bool(true)
40642
-- Iteration 420 --
bool(true)
100643
bool(true)
40643
-- Iteration 421 --
bool(true)
100644
bool(true)
40644
-- Iteration 422 --
bool(true)
100645
bool(true)
40645
-- Iteration 423 --
bool(true)
100646
bool(true)
40646
-- Iteration 424 --
bool(true)
100647
bool(true)
40647
-- Iteration 425 --
bool(true)
100650
bool(true)
40650
-- Iteration 426 --
bool(true)
100651
bool(true)
40651
-- Iteration 427 --
bool(true)
100652
bool(true)
40652
-- Iteration 428 --
bool(true)
100653
bool(true)
40653
-- Iteration 429 --
bool(true)
100654
bool(true)
40654
-- Iteration 430 --
bool(true)
100655
bool(true)
40655
-- Iteration 431 --
bool(true)
100656
bool(true)
40656
-- Iteration 432 --
bool(true)
100657
bool(true)
40657
-- Iteration 433 --
bool(true)
100660
bool(true)
40660
-- Iteration 434 --
bool(true)
100661
bool(true)
40661
-- Iteration 435 --
bool(true)
100662
bool(true)
40662
-- Iteration 436 --
bool(true)
100663
bool(true)
40663
-- Iteration 437 --
bool(true)
100664
bool(true)
40664
-- Iteration 438 --
bool(true)
100665
bool(true)
40665
-- Iteration 439 --
bool(true)
100666
bool(true)
40666
-- Iteration 440 --
bool(true)
100667
bool(true)
40667
-- Iteration 441 --
bool(true)
100670
bool(true)
40670
-- Iteration 442 --
bool(true)
100671
bool(true)
40671
-- Iteration 443 --
bool(true)
100672
bool(true)
40672
-- Iteration 444 --
bool(true)
100673
bool(true)
40673
-- Iteration 445 --
bool(true)
100674
bool(true)
40674
-- Iteration 446 --
bool(true)
100675
bool(true)
40675
-- Iteration 447 --
bool(true)
100676
bool(true)
40676
-- Iteration 448 --
bool(true)
100677
bool(true)
40677
-- Iteration 449 --
bool(true)
100700
bool(true)
40700
-- Iteration 450 --
bool(true)
100701
bool(true)
40701
-- Iteration 451 --
bool(true)
100702
bool(true)
40702
-- Iteration 452 --
bool(true)
100703
bool(true)
40703
-- Iteration 453 --
bool(true)
100704
bool(true)
40704
-- Iteration 454 --
bool(true)
100705
bool(true)
40705
-- Iteration 455 --
bool(true)
100706
bool(true)
40706
-- Iteration 456 --
bool(true)
100707
bool(true)
40707
-- Iteration 457 --
bool(true)
100710
bool(true)
40710
-- Iteration 458 --
bool(true)
100711
bool(true)
40711
-- Iteration 459 --
bool(true)
100712
bool(true)
40712
-- Iteration 460 --
bool(true)
100713
bool(true)
40713
-- Iteration 461 --
bool(true)
100714
bool(true)
40714
-- Iteration 462 --
bool(true)
100715
bool(true)
40715
-- Iteration 463 --
bool(true)
100716
bool(true)
40716
-- Iteration 464 --
bool(true)
100717
bool(true)
40717
-- Iteration 465 --
bool(true)
100720
bool(true)
40720
-- Iteration 466 --
bool(true)
100721
bool(true)
40721
-- Iteration 467 --
bool(true)
100722
bool(true)
40722
-- Iteration 468 --
bool(true)
100723
bool(true)
40723
-- Iteration 469 --
bool(true)
100724
bool(true)
40724
-- Iteration 470 --
bool(true)
100725
bool(true)
40725
-- Iteration 471 --
bool(true)
100726
bool(true)
40726
-- Iteration 472 --
bool(true)
100727
bool(true)
40727
-- Iteration 473 --
bool(true)
100730
bool(true)
40730
-- Iteration 474 --
bool(true)
100731
bool(true)
40731
-- Iteration 475 --
bool(true)
100732
bool(true)
40732
-- Iteration 476 --
bool(true)
100733
bool(true)
40733
-- Iteration 477 --
bool(true)
100734
bool(true)
40734
-- Iteration 478 --
bool(true)
100735
bool(true)
40735
-- Iteration 479 --
bool(true)
100736
bool(true)
40736
-- Iteration 480 --
bool(true)
100737
bool(true)
40737
-- Iteration 481 --
bool(true)
100740
bool(true)
40740
-- Iteration 482 --
bool(true)
100741
bool(true)
40741
-- Iteration 483 --
bool(true)
100742
bool(true)
40742
-- Iteration 484 --
bool(true)
100743
bool(true)
40743
-- Iteration 485 --
bool(true)
100744
bool(true)
40744
-- Iteration 486 --
bool(true)
100745
bool(true)
40745
-- Iteration 487 --
bool(true)
100746
bool(true)
40746
-- Iteration 488 --
bool(true)
100747
bool(true)
40747
-- Iteration 489 --
bool(true)
100750
bool(true)
40750
-- Iteration 490 --
bool(true)
100751
bool(true)
40751
-- Iteration 491 --
bool(true)
100752
bool(true)
40752
-- Iteration 492 --
bool(true)
100753
bool(true)
40753
-- Iteration 493 --
bool(true)
100754
bool(true)
40754
-- Iteration 494 --
bool(true)
100755
bool(true)
40755
-- Iteration 495 --
bool(true)
100756
bool(true)
40756
-- Iteration 496 --
bool(true)
100757
bool(true)
40757
-- Iteration 497 --
bool(true)
100760
bool(true)
40760
-- Iteration 498 --
bool(true)
100761
bool(true)
40761
-- Iteration 499 --
bool(true)
100762
bool(true)
40762
-- Iteration 500 --
bool(true)
100763
bool(true)
40763
-- Iteration 501 --
bool(true)
100764
bool(true)
40764
-- Iteration 502 --
bool(true)
100765
bool(true)
40765
-- Iteration 503 --
bool(true)
100766
bool(true)
40766
-- Iteration 504 --
bool(true)
100767
bool(true)
40767
-- Iteration 505 --
bool(true)
100770
bool(true)
40770
-- Iteration 506 --
bool(true)
100771
bool(true)
40771
-- Iteration 507 --
bool(true)
100772
bool(true)
40772
-- Iteration 508 --
bool(true)
100773
bool(true)
40773
-- Iteration 509 --
bool(true)
100774
bool(true)
40774
-- Iteration 510 --
bool(true)
100775
bool(true)
40775
-- Iteration 511 --
bool(true)
100776
bool(true)
40776
-- Iteration 512 --
bool(true)
100777
bool(true)
40777
*** Done ***
