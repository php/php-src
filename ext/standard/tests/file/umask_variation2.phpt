--TEST--
Test umask() function: usage variations - perms from 0351 to 0777
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only on Linux');
}
?>
--FILE--
<?php
/* Prototype: int umask ( [int $mask] );
   Description: Changes the current umask
*/

$file_path = __DIR__;

/* Check umask() on file/dir */

echo "*** Testing umask() on file and directory ***\n";
// temp filename used
$filename = "$file_path/umask_variation2.tmp";
// temp dir used
$dirname = "$file_path/umask_variation2";

for($mask = 0351; $mask <= 0777; $mask++) {
  echo "-- Setting umask to ";
  echo sprintf('%03o', $mask);
  echo " --\n";
  // setting umask
  umask($mask);

  /* umasking file */
  // creating temp file
  $fp = fopen($filename, "w");
  fclose($fp);
  echo "File permission : ";
  // check file permission
  echo substr(sprintf('%o', fileperms($filename)), -3);
  echo "\n";
  // chmod file to 0777 to enable deletion
  chmod($filename, 0777);
  // delete temp file created here
  unlink($filename);

  /* umasking directory */
  // create temp dir
  mkdir($dirname);
  echo "Directory permission : ";
  // check $dirname permission
  echo substr(sprintf('%o', fileperms($dirname)), -3);
  echo "\n";
  // chmod 0777 to enable deletion
  chmod($dirname, 0777);
  // delete temp dir created
  rmdir($dirname);
}

echo "Done\n";
?>
--EXPECT--
*** Testing umask() on file and directory ***
-- Setting umask to 351 --
File permission : 426
Directory permission : 426
-- Setting umask to 352 --
File permission : 424
Directory permission : 425
-- Setting umask to 353 --
File permission : 424
Directory permission : 424
-- Setting umask to 354 --
File permission : 422
Directory permission : 423
-- Setting umask to 355 --
File permission : 422
Directory permission : 422
-- Setting umask to 356 --
File permission : 420
Directory permission : 421
-- Setting umask to 357 --
File permission : 420
Directory permission : 420
-- Setting umask to 360 --
File permission : 406
Directory permission : 417
-- Setting umask to 361 --
File permission : 406
Directory permission : 416
-- Setting umask to 362 --
File permission : 404
Directory permission : 415
-- Setting umask to 363 --
File permission : 404
Directory permission : 414
-- Setting umask to 364 --
File permission : 402
Directory permission : 413
-- Setting umask to 365 --
File permission : 402
Directory permission : 412
-- Setting umask to 366 --
File permission : 400
Directory permission : 411
-- Setting umask to 367 --
File permission : 400
Directory permission : 410
-- Setting umask to 370 --
File permission : 406
Directory permission : 407
-- Setting umask to 371 --
File permission : 406
Directory permission : 406
-- Setting umask to 372 --
File permission : 404
Directory permission : 405
-- Setting umask to 373 --
File permission : 404
Directory permission : 404
-- Setting umask to 374 --
File permission : 402
Directory permission : 403
-- Setting umask to 375 --
File permission : 402
Directory permission : 402
-- Setting umask to 376 --
File permission : 400
Directory permission : 401
-- Setting umask to 377 --
File permission : 400
Directory permission : 400
-- Setting umask to 400 --
File permission : 266
Directory permission : 377
-- Setting umask to 401 --
File permission : 266
Directory permission : 376
-- Setting umask to 402 --
File permission : 264
Directory permission : 375
-- Setting umask to 403 --
File permission : 264
Directory permission : 374
-- Setting umask to 404 --
File permission : 262
Directory permission : 373
-- Setting umask to 405 --
File permission : 262
Directory permission : 372
-- Setting umask to 406 --
File permission : 260
Directory permission : 371
-- Setting umask to 407 --
File permission : 260
Directory permission : 370
-- Setting umask to 410 --
File permission : 266
Directory permission : 367
-- Setting umask to 411 --
File permission : 266
Directory permission : 366
-- Setting umask to 412 --
File permission : 264
Directory permission : 365
-- Setting umask to 413 --
File permission : 264
Directory permission : 364
-- Setting umask to 414 --
File permission : 262
Directory permission : 363
-- Setting umask to 415 --
File permission : 262
Directory permission : 362
-- Setting umask to 416 --
File permission : 260
Directory permission : 361
-- Setting umask to 417 --
File permission : 260
Directory permission : 360
-- Setting umask to 420 --
File permission : 246
Directory permission : 357
-- Setting umask to 421 --
File permission : 246
Directory permission : 356
-- Setting umask to 422 --
File permission : 244
Directory permission : 355
-- Setting umask to 423 --
File permission : 244
Directory permission : 354
-- Setting umask to 424 --
File permission : 242
Directory permission : 353
-- Setting umask to 425 --
File permission : 242
Directory permission : 352
-- Setting umask to 426 --
File permission : 240
Directory permission : 351
-- Setting umask to 427 --
File permission : 240
Directory permission : 350
-- Setting umask to 430 --
File permission : 246
Directory permission : 347
-- Setting umask to 431 --
File permission : 246
Directory permission : 346
-- Setting umask to 432 --
File permission : 244
Directory permission : 345
-- Setting umask to 433 --
File permission : 244
Directory permission : 344
-- Setting umask to 434 --
File permission : 242
Directory permission : 343
-- Setting umask to 435 --
File permission : 242
Directory permission : 342
-- Setting umask to 436 --
File permission : 240
Directory permission : 341
-- Setting umask to 437 --
File permission : 240
Directory permission : 340
-- Setting umask to 440 --
File permission : 226
Directory permission : 337
-- Setting umask to 441 --
File permission : 226
Directory permission : 336
-- Setting umask to 442 --
File permission : 224
Directory permission : 335
-- Setting umask to 443 --
File permission : 224
Directory permission : 334
-- Setting umask to 444 --
File permission : 222
Directory permission : 333
-- Setting umask to 445 --
File permission : 222
Directory permission : 332
-- Setting umask to 446 --
File permission : 220
Directory permission : 331
-- Setting umask to 447 --
File permission : 220
Directory permission : 330
-- Setting umask to 450 --
File permission : 226
Directory permission : 327
-- Setting umask to 451 --
File permission : 226
Directory permission : 326
-- Setting umask to 452 --
File permission : 224
Directory permission : 325
-- Setting umask to 453 --
File permission : 224
Directory permission : 324
-- Setting umask to 454 --
File permission : 222
Directory permission : 323
-- Setting umask to 455 --
File permission : 222
Directory permission : 322
-- Setting umask to 456 --
File permission : 220
Directory permission : 321
-- Setting umask to 457 --
File permission : 220
Directory permission : 320
-- Setting umask to 460 --
File permission : 206
Directory permission : 317
-- Setting umask to 461 --
File permission : 206
Directory permission : 316
-- Setting umask to 462 --
File permission : 204
Directory permission : 315
-- Setting umask to 463 --
File permission : 204
Directory permission : 314
-- Setting umask to 464 --
File permission : 202
Directory permission : 313
-- Setting umask to 465 --
File permission : 202
Directory permission : 312
-- Setting umask to 466 --
File permission : 200
Directory permission : 311
-- Setting umask to 467 --
File permission : 200
Directory permission : 310
-- Setting umask to 470 --
File permission : 206
Directory permission : 307
-- Setting umask to 471 --
File permission : 206
Directory permission : 306
-- Setting umask to 472 --
File permission : 204
Directory permission : 305
-- Setting umask to 473 --
File permission : 204
Directory permission : 304
-- Setting umask to 474 --
File permission : 202
Directory permission : 303
-- Setting umask to 475 --
File permission : 202
Directory permission : 302
-- Setting umask to 476 --
File permission : 200
Directory permission : 301
-- Setting umask to 477 --
File permission : 200
Directory permission : 300
-- Setting umask to 500 --
File permission : 266
Directory permission : 277
-- Setting umask to 501 --
File permission : 266
Directory permission : 276
-- Setting umask to 502 --
File permission : 264
Directory permission : 275
-- Setting umask to 503 --
File permission : 264
Directory permission : 274
-- Setting umask to 504 --
File permission : 262
Directory permission : 273
-- Setting umask to 505 --
File permission : 262
Directory permission : 272
-- Setting umask to 506 --
File permission : 260
Directory permission : 271
-- Setting umask to 507 --
File permission : 260
Directory permission : 270
-- Setting umask to 510 --
File permission : 266
Directory permission : 267
-- Setting umask to 511 --
File permission : 266
Directory permission : 266
-- Setting umask to 512 --
File permission : 264
Directory permission : 265
-- Setting umask to 513 --
File permission : 264
Directory permission : 264
-- Setting umask to 514 --
File permission : 262
Directory permission : 263
-- Setting umask to 515 --
File permission : 262
Directory permission : 262
-- Setting umask to 516 --
File permission : 260
Directory permission : 261
-- Setting umask to 517 --
File permission : 260
Directory permission : 260
-- Setting umask to 520 --
File permission : 246
Directory permission : 257
-- Setting umask to 521 --
File permission : 246
Directory permission : 256
-- Setting umask to 522 --
File permission : 244
Directory permission : 255
-- Setting umask to 523 --
File permission : 244
Directory permission : 254
-- Setting umask to 524 --
File permission : 242
Directory permission : 253
-- Setting umask to 525 --
File permission : 242
Directory permission : 252
-- Setting umask to 526 --
File permission : 240
Directory permission : 251
-- Setting umask to 527 --
File permission : 240
Directory permission : 250
-- Setting umask to 530 --
File permission : 246
Directory permission : 247
-- Setting umask to 531 --
File permission : 246
Directory permission : 246
-- Setting umask to 532 --
File permission : 244
Directory permission : 245
-- Setting umask to 533 --
File permission : 244
Directory permission : 244
-- Setting umask to 534 --
File permission : 242
Directory permission : 243
-- Setting umask to 535 --
File permission : 242
Directory permission : 242
-- Setting umask to 536 --
File permission : 240
Directory permission : 241
-- Setting umask to 537 --
File permission : 240
Directory permission : 240
-- Setting umask to 540 --
File permission : 226
Directory permission : 237
-- Setting umask to 541 --
File permission : 226
Directory permission : 236
-- Setting umask to 542 --
File permission : 224
Directory permission : 235
-- Setting umask to 543 --
File permission : 224
Directory permission : 234
-- Setting umask to 544 --
File permission : 222
Directory permission : 233
-- Setting umask to 545 --
File permission : 222
Directory permission : 232
-- Setting umask to 546 --
File permission : 220
Directory permission : 231
-- Setting umask to 547 --
File permission : 220
Directory permission : 230
-- Setting umask to 550 --
File permission : 226
Directory permission : 227
-- Setting umask to 551 --
File permission : 226
Directory permission : 226
-- Setting umask to 552 --
File permission : 224
Directory permission : 225
-- Setting umask to 553 --
File permission : 224
Directory permission : 224
-- Setting umask to 554 --
File permission : 222
Directory permission : 223
-- Setting umask to 555 --
File permission : 222
Directory permission : 222
-- Setting umask to 556 --
File permission : 220
Directory permission : 221
-- Setting umask to 557 --
File permission : 220
Directory permission : 220
-- Setting umask to 560 --
File permission : 206
Directory permission : 217
-- Setting umask to 561 --
File permission : 206
Directory permission : 216
-- Setting umask to 562 --
File permission : 204
Directory permission : 215
-- Setting umask to 563 --
File permission : 204
Directory permission : 214
-- Setting umask to 564 --
File permission : 202
Directory permission : 213
-- Setting umask to 565 --
File permission : 202
Directory permission : 212
-- Setting umask to 566 --
File permission : 200
Directory permission : 211
-- Setting umask to 567 --
File permission : 200
Directory permission : 210
-- Setting umask to 570 --
File permission : 206
Directory permission : 207
-- Setting umask to 571 --
File permission : 206
Directory permission : 206
-- Setting umask to 572 --
File permission : 204
Directory permission : 205
-- Setting umask to 573 --
File permission : 204
Directory permission : 204
-- Setting umask to 574 --
File permission : 202
Directory permission : 203
-- Setting umask to 575 --
File permission : 202
Directory permission : 202
-- Setting umask to 576 --
File permission : 200
Directory permission : 201
-- Setting umask to 577 --
File permission : 200
Directory permission : 200
-- Setting umask to 600 --
File permission : 066
Directory permission : 177
-- Setting umask to 601 --
File permission : 066
Directory permission : 176
-- Setting umask to 602 --
File permission : 064
Directory permission : 175
-- Setting umask to 603 --
File permission : 064
Directory permission : 174
-- Setting umask to 604 --
File permission : 062
Directory permission : 173
-- Setting umask to 605 --
File permission : 062
Directory permission : 172
-- Setting umask to 606 --
File permission : 060
Directory permission : 171
-- Setting umask to 607 --
File permission : 060
Directory permission : 170
-- Setting umask to 610 --
File permission : 066
Directory permission : 167
-- Setting umask to 611 --
File permission : 066
Directory permission : 166
-- Setting umask to 612 --
File permission : 064
Directory permission : 165
-- Setting umask to 613 --
File permission : 064
Directory permission : 164
-- Setting umask to 614 --
File permission : 062
Directory permission : 163
-- Setting umask to 615 --
File permission : 062
Directory permission : 162
-- Setting umask to 616 --
File permission : 060
Directory permission : 161
-- Setting umask to 617 --
File permission : 060
Directory permission : 160
-- Setting umask to 620 --
File permission : 046
Directory permission : 157
-- Setting umask to 621 --
File permission : 046
Directory permission : 156
-- Setting umask to 622 --
File permission : 044
Directory permission : 155
-- Setting umask to 623 --
File permission : 044
Directory permission : 154
-- Setting umask to 624 --
File permission : 042
Directory permission : 153
-- Setting umask to 625 --
File permission : 042
Directory permission : 152
-- Setting umask to 626 --
File permission : 040
Directory permission : 151
-- Setting umask to 627 --
File permission : 040
Directory permission : 150
-- Setting umask to 630 --
File permission : 046
Directory permission : 147
-- Setting umask to 631 --
File permission : 046
Directory permission : 146
-- Setting umask to 632 --
File permission : 044
Directory permission : 145
-- Setting umask to 633 --
File permission : 044
Directory permission : 144
-- Setting umask to 634 --
File permission : 042
Directory permission : 143
-- Setting umask to 635 --
File permission : 042
Directory permission : 142
-- Setting umask to 636 --
File permission : 040
Directory permission : 141
-- Setting umask to 637 --
File permission : 040
Directory permission : 140
-- Setting umask to 640 --
File permission : 026
Directory permission : 137
-- Setting umask to 641 --
File permission : 026
Directory permission : 136
-- Setting umask to 642 --
File permission : 024
Directory permission : 135
-- Setting umask to 643 --
File permission : 024
Directory permission : 134
-- Setting umask to 644 --
File permission : 022
Directory permission : 133
-- Setting umask to 645 --
File permission : 022
Directory permission : 132
-- Setting umask to 646 --
File permission : 020
Directory permission : 131
-- Setting umask to 647 --
File permission : 020
Directory permission : 130
-- Setting umask to 650 --
File permission : 026
Directory permission : 127
-- Setting umask to 651 --
File permission : 026
Directory permission : 126
-- Setting umask to 652 --
File permission : 024
Directory permission : 125
-- Setting umask to 653 --
File permission : 024
Directory permission : 124
-- Setting umask to 654 --
File permission : 022
Directory permission : 123
-- Setting umask to 655 --
File permission : 022
Directory permission : 122
-- Setting umask to 656 --
File permission : 020
Directory permission : 121
-- Setting umask to 657 --
File permission : 020
Directory permission : 120
-- Setting umask to 660 --
File permission : 006
Directory permission : 117
-- Setting umask to 661 --
File permission : 006
Directory permission : 116
-- Setting umask to 662 --
File permission : 004
Directory permission : 115
-- Setting umask to 663 --
File permission : 004
Directory permission : 114
-- Setting umask to 664 --
File permission : 002
Directory permission : 113
-- Setting umask to 665 --
File permission : 002
Directory permission : 112
-- Setting umask to 666 --
File permission : 000
Directory permission : 111
-- Setting umask to 667 --
File permission : 000
Directory permission : 110
-- Setting umask to 670 --
File permission : 006
Directory permission : 107
-- Setting umask to 671 --
File permission : 006
Directory permission : 106
-- Setting umask to 672 --
File permission : 004
Directory permission : 105
-- Setting umask to 673 --
File permission : 004
Directory permission : 104
-- Setting umask to 674 --
File permission : 002
Directory permission : 103
-- Setting umask to 675 --
File permission : 002
Directory permission : 102
-- Setting umask to 676 --
File permission : 000
Directory permission : 101
-- Setting umask to 677 --
File permission : 000
Directory permission : 100
-- Setting umask to 700 --
File permission : 066
Directory permission : 077
-- Setting umask to 701 --
File permission : 066
Directory permission : 076
-- Setting umask to 702 --
File permission : 064
Directory permission : 075
-- Setting umask to 703 --
File permission : 064
Directory permission : 074
-- Setting umask to 704 --
File permission : 062
Directory permission : 073
-- Setting umask to 705 --
File permission : 062
Directory permission : 072
-- Setting umask to 706 --
File permission : 060
Directory permission : 071
-- Setting umask to 707 --
File permission : 060
Directory permission : 070
-- Setting umask to 710 --
File permission : 066
Directory permission : 067
-- Setting umask to 711 --
File permission : 066
Directory permission : 066
-- Setting umask to 712 --
File permission : 064
Directory permission : 065
-- Setting umask to 713 --
File permission : 064
Directory permission : 064
-- Setting umask to 714 --
File permission : 062
Directory permission : 063
-- Setting umask to 715 --
File permission : 062
Directory permission : 062
-- Setting umask to 716 --
File permission : 060
Directory permission : 061
-- Setting umask to 717 --
File permission : 060
Directory permission : 060
-- Setting umask to 720 --
File permission : 046
Directory permission : 057
-- Setting umask to 721 --
File permission : 046
Directory permission : 056
-- Setting umask to 722 --
File permission : 044
Directory permission : 055
-- Setting umask to 723 --
File permission : 044
Directory permission : 054
-- Setting umask to 724 --
File permission : 042
Directory permission : 053
-- Setting umask to 725 --
File permission : 042
Directory permission : 052
-- Setting umask to 726 --
File permission : 040
Directory permission : 051
-- Setting umask to 727 --
File permission : 040
Directory permission : 050
-- Setting umask to 730 --
File permission : 046
Directory permission : 047
-- Setting umask to 731 --
File permission : 046
Directory permission : 046
-- Setting umask to 732 --
File permission : 044
Directory permission : 045
-- Setting umask to 733 --
File permission : 044
Directory permission : 044
-- Setting umask to 734 --
File permission : 042
Directory permission : 043
-- Setting umask to 735 --
File permission : 042
Directory permission : 042
-- Setting umask to 736 --
File permission : 040
Directory permission : 041
-- Setting umask to 737 --
File permission : 040
Directory permission : 040
-- Setting umask to 740 --
File permission : 026
Directory permission : 037
-- Setting umask to 741 --
File permission : 026
Directory permission : 036
-- Setting umask to 742 --
File permission : 024
Directory permission : 035
-- Setting umask to 743 --
File permission : 024
Directory permission : 034
-- Setting umask to 744 --
File permission : 022
Directory permission : 033
-- Setting umask to 745 --
File permission : 022
Directory permission : 032
-- Setting umask to 746 --
File permission : 020
Directory permission : 031
-- Setting umask to 747 --
File permission : 020
Directory permission : 030
-- Setting umask to 750 --
File permission : 026
Directory permission : 027
-- Setting umask to 751 --
File permission : 026
Directory permission : 026
-- Setting umask to 752 --
File permission : 024
Directory permission : 025
-- Setting umask to 753 --
File permission : 024
Directory permission : 024
-- Setting umask to 754 --
File permission : 022
Directory permission : 023
-- Setting umask to 755 --
File permission : 022
Directory permission : 022
-- Setting umask to 756 --
File permission : 020
Directory permission : 021
-- Setting umask to 757 --
File permission : 020
Directory permission : 020
-- Setting umask to 760 --
File permission : 006
Directory permission : 017
-- Setting umask to 761 --
File permission : 006
Directory permission : 016
-- Setting umask to 762 --
File permission : 004
Directory permission : 015
-- Setting umask to 763 --
File permission : 004
Directory permission : 014
-- Setting umask to 764 --
File permission : 002
Directory permission : 013
-- Setting umask to 765 --
File permission : 002
Directory permission : 012
-- Setting umask to 766 --
File permission : 000
Directory permission : 011
-- Setting umask to 767 --
File permission : 000
Directory permission : 010
-- Setting umask to 770 --
File permission : 006
Directory permission : 007
-- Setting umask to 771 --
File permission : 006
Directory permission : 006
-- Setting umask to 772 --
File permission : 004
Directory permission : 005
-- Setting umask to 773 --
File permission : 004
Directory permission : 004
-- Setting umask to 774 --
File permission : 002
Directory permission : 003
-- Setting umask to 775 --
File permission : 002
Directory permission : 002
-- Setting umask to 776 --
File permission : 000
Directory permission : 001
-- Setting umask to 777 --
File permission : 000
Directory permission : 000
Done
