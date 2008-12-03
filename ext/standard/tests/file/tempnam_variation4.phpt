--TEST--
Test tempnam() function: usage variations - permissions(0000 to 0777) of dir
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not valid for Windows');
}
?>
--FILE--
<?php
/* Prototype:  string tempnam ( string $dir, string $prefix );
   Description: Create file with unique file name.
*/

/* Trying to create the file in a dir with permissions from 0000 to 0777,
     Allowable permissions: files are expected to be created in the input dir 
     Non-allowable permissions: files are expected to be created in '/tmp' dir
*/

echo "*** Testing tempnam() with dir of permissions from 0000 to 0777 ***\n";
$file_path = dirname(__FILE__);
$dir_name = $file_path."/tempnam_variation4";
mkdir($dir_name);

for($mode = 0000; $mode<=0777; $mode++) {
  echo "-- dir perms ";
  printf("%o", $mode);
  echo " --\n";
  chmod($dir_name, $mode);
  $file_name = tempnam($dir_name, "tempnam_variation4.tmp");

  if(file_exists($file_name) ) {
    print($file_name);
    echo "\n";
  }
  else
    print("-- File is not created --");
  unlink($file_name);
}

rmdir($dir_name);

echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing tempnam() with dir of permissions from 0000 to 0777 ***
-- dir perms 0 --
%s
-- dir perms 1 --
%s
-- dir perms 2 --
%s
-- dir perms 3 --
%s
-- dir perms 4 --
%s
-- dir perms 5 --
%s
-- dir perms 6 --
%s
-- dir perms 7 --
%s
-- dir perms 10 --
%s
-- dir perms 11 --
%s
-- dir perms 12 --
%s
-- dir perms 13 --
%s
-- dir perms 14 --
%s
-- dir perms 15 --
%s
-- dir perms 16 --
%s
-- dir perms 17 --
%s
-- dir perms 20 --
%s
-- dir perms 21 --
%s
-- dir perms 22 --
%s
-- dir perms 23 --
%s
-- dir perms 24 --
%s
-- dir perms 25 --
%s
-- dir perms 26 --
%s
-- dir perms 27 --
%s
-- dir perms 30 --
%s
-- dir perms 31 --
%s
-- dir perms 32 --
%s
-- dir perms 33 --
%s
-- dir perms 34 --
%s
-- dir perms 35 --
%s
-- dir perms 36 --
%s
-- dir perms 37 --
%s
-- dir perms 40 --
%s
-- dir perms 41 --
%s
-- dir perms 42 --
%s
-- dir perms 43 --
%s
-- dir perms 44 --
%s
-- dir perms 45 --
%s
-- dir perms 46 --
%s
-- dir perms 47 --
%s
-- dir perms 50 --
%s
-- dir perms 51 --
%s
-- dir perms 52 --
%s
-- dir perms 53 --
%s
-- dir perms 54 --
%s
-- dir perms 55 --
%s
-- dir perms 56 --
%s
-- dir perms 57 --
%s
-- dir perms 60 --
%s
-- dir perms 61 --
%s
-- dir perms 62 --
%s
-- dir perms 63 --
%s
-- dir perms 64 --
%s
-- dir perms 65 --
%s
-- dir perms 66 --
%s
-- dir perms 67 --
%s
-- dir perms 70 --
%s
-- dir perms 71 --
%s
-- dir perms 72 --
%s
-- dir perms 73 --
%s
-- dir perms 74 --
%s
-- dir perms 75 --
%s
-- dir perms 76 --
%s
-- dir perms 77 --
%s
-- dir perms 100 --
%s
-- dir perms 101 --
%s
-- dir perms 102 --
%s
-- dir perms 103 --
%s
-- dir perms 104 --
%s
-- dir perms 105 --
%s
-- dir perms 106 --
%s
-- dir perms 107 --
%s
-- dir perms 110 --
%s
-- dir perms 111 --
%s
-- dir perms 112 --
%s
-- dir perms 113 --
%s
-- dir perms 114 --
%s
-- dir perms 115 --
%s
-- dir perms 116 --
%s
-- dir perms 117 --
%s
-- dir perms 120 --
%s
-- dir perms 121 --
%s
-- dir perms 122 --
%s
-- dir perms 123 --
%s
-- dir perms 124 --
%s
-- dir perms 125 --
%s
-- dir perms 126 --
%s
-- dir perms 127 --
%s
-- dir perms 130 --
%s
-- dir perms 131 --
%s
-- dir perms 132 --
%s
-- dir perms 133 --
%s
-- dir perms 134 --
%s
-- dir perms 135 --
%s
-- dir perms 136 --
%s
-- dir perms 137 --
%s
-- dir perms 140 --
%s
-- dir perms 141 --
%s
-- dir perms 142 --
%s
-- dir perms 143 --
%s
-- dir perms 144 --
%s
-- dir perms 145 --
%s
-- dir perms 146 --
%s
-- dir perms 147 --
%s
-- dir perms 150 --
%s
-- dir perms 151 --
%s
-- dir perms 152 --
%s
-- dir perms 153 --
%s
-- dir perms 154 --
%s
-- dir perms 155 --
%s
-- dir perms 156 --
%s
-- dir perms 157 --
%s
-- dir perms 160 --
%s
-- dir perms 161 --
%s
-- dir perms 162 --
%s
-- dir perms 163 --
%s
-- dir perms 164 --
%s
-- dir perms 165 --
%s
-- dir perms 166 --
%s
-- dir perms 167 --
%s
-- dir perms 170 --
%s
-- dir perms 171 --
%s
-- dir perms 172 --
%s
-- dir perms 173 --
%s
-- dir perms 174 --
%s
-- dir perms 175 --
%s
-- dir perms 176 --
%s
-- dir perms 177 --
%s
-- dir perms 200 --
%s
-- dir perms 201 --
%s
-- dir perms 202 --
%s
-- dir perms 203 --
%s
-- dir perms 204 --
%s
-- dir perms 205 --
%s
-- dir perms 206 --
%s
-- dir perms 207 --
%s
-- dir perms 210 --
%s
-- dir perms 211 --
%s
-- dir perms 212 --
%s
-- dir perms 213 --
%s
-- dir perms 214 --
%s
-- dir perms 215 --
%s
-- dir perms 216 --
%s
-- dir perms 217 --
%s
-- dir perms 220 --
%s
-- dir perms 221 --
%s
-- dir perms 222 --
%s
-- dir perms 223 --
%s
-- dir perms 224 --
%s
-- dir perms 225 --
%s
-- dir perms 226 --
%s
-- dir perms 227 --
%s
-- dir perms 230 --
%s
-- dir perms 231 --
%s
-- dir perms 232 --
%s
-- dir perms 233 --
%s
-- dir perms 234 --
%s
-- dir perms 235 --
%s
-- dir perms 236 --
%s
-- dir perms 237 --
%s
-- dir perms 240 --
%s
-- dir perms 241 --
%s
-- dir perms 242 --
%s
-- dir perms 243 --
%s
-- dir perms 244 --
%s
-- dir perms 245 --
%s
-- dir perms 246 --
%s
-- dir perms 247 --
%s
-- dir perms 250 --
%s
-- dir perms 251 --
%s
-- dir perms 252 --
%s
-- dir perms 253 --
%s
-- dir perms 254 --
%s
-- dir perms 255 --
%s
-- dir perms 256 --
%s
-- dir perms 257 --
%s
-- dir perms 260 --
%s
-- dir perms 261 --
%s
-- dir perms 262 --
%s
-- dir perms 263 --
%s
-- dir perms 264 --
%s
-- dir perms 265 --
%s
-- dir perms 266 --
%s
-- dir perms 267 --
%s
-- dir perms 270 --
%s
-- dir perms 271 --
%s
-- dir perms 272 --
%s
-- dir perms 273 --
%s
-- dir perms 274 --
%s
-- dir perms 275 --
%s
-- dir perms 276 --
%s
-- dir perms 277 --
%s
-- dir perms 300 --
%s
-- dir perms 301 --
%s
-- dir perms 302 --
%s
-- dir perms 303 --
%s
-- dir perms 304 --
%s
-- dir perms 305 --
%s
-- dir perms 306 --
%s
-- dir perms 307 --
%s
-- dir perms 310 --
%s
-- dir perms 311 --
%s
-- dir perms 312 --
%s
-- dir perms 313 --
%s
-- dir perms 314 --
%s
-- dir perms 315 --
%s
-- dir perms 316 --
%s
-- dir perms 317 --
%s
-- dir perms 320 --
%s
-- dir perms 321 --
%s
-- dir perms 322 --
%s
-- dir perms 323 --
%s
-- dir perms 324 --
%s
-- dir perms 325 --
%s
-- dir perms 326 --
%s
-- dir perms 327 --
%s
-- dir perms 330 --
%s
-- dir perms 331 --
%s
-- dir perms 332 --
%s
-- dir perms 333 --
%s
-- dir perms 334 --
%s
-- dir perms 335 --
%s
-- dir perms 336 --
%s
-- dir perms 337 --
%s
-- dir perms 340 --
%s
-- dir perms 341 --
%s
-- dir perms 342 --
%s
-- dir perms 343 --
%s
-- dir perms 344 --
%s
-- dir perms 345 --
%s
-- dir perms 346 --
%s
-- dir perms 347 --
%s
-- dir perms 350 --
%s
-- dir perms 351 --
%s
-- dir perms 352 --
%s
-- dir perms 353 --
%s
-- dir perms 354 --
%s
-- dir perms 355 --
%s
-- dir perms 356 --
%s
-- dir perms 357 --
%s
-- dir perms 360 --
%s
-- dir perms 361 --
%s
-- dir perms 362 --
%s
-- dir perms 363 --
%s
-- dir perms 364 --
%s
-- dir perms 365 --
%s
-- dir perms 366 --
%s
-- dir perms 367 --
%s
-- dir perms 370 --
%s
-- dir perms 371 --
%s
-- dir perms 372 --
%s
-- dir perms 373 --
%s
-- dir perms 374 --
%s
-- dir perms 375 --
%s
-- dir perms 376 --
%s
-- dir perms 377 --
%s
-- dir perms 400 --
%s
-- dir perms 401 --
%s
-- dir perms 402 --
%s
-- dir perms 403 --
%s
-- dir perms 404 --
%s
-- dir perms 405 --
%s
-- dir perms 406 --
%s
-- dir perms 407 --
%s
-- dir perms 410 --
%s
-- dir perms 411 --
%s
-- dir perms 412 --
%s
-- dir perms 413 --
%s
-- dir perms 414 --
%s
-- dir perms 415 --
%s
-- dir perms 416 --
%s
-- dir perms 417 --
%s
-- dir perms 420 --
%s
-- dir perms 421 --
%s
-- dir perms 422 --
%s
-- dir perms 423 --
%s
-- dir perms 424 --
%s
-- dir perms 425 --
%s
-- dir perms 426 --
%s
-- dir perms 427 --
%s
-- dir perms 430 --
%s
-- dir perms 431 --
%s
-- dir perms 432 --
%s
-- dir perms 433 --
%s
-- dir perms 434 --
%s
-- dir perms 435 --
%s
-- dir perms 436 --
%s
-- dir perms 437 --
%s
-- dir perms 440 --
%s
-- dir perms 441 --
%s
-- dir perms 442 --
%s
-- dir perms 443 --
%s
-- dir perms 444 --
%s
-- dir perms 445 --
%s
-- dir perms 446 --
%s
-- dir perms 447 --
%s
-- dir perms 450 --
%s
-- dir perms 451 --
%s
-- dir perms 452 --
%s
-- dir perms 453 --
%s
-- dir perms 454 --
%s
-- dir perms 455 --
%s
-- dir perms 456 --
%s
-- dir perms 457 --
%s
-- dir perms 460 --
%s
-- dir perms 461 --
%s
-- dir perms 462 --
%s
-- dir perms 463 --
%s
-- dir perms 464 --
%s
-- dir perms 465 --
%s
-- dir perms 466 --
%s
-- dir perms 467 --
%s
-- dir perms 470 --
%s
-- dir perms 471 --
%s
-- dir perms 472 --
%s
-- dir perms 473 --
%s
-- dir perms 474 --
%s
-- dir perms 475 --
%s
-- dir perms 476 --
%s
-- dir perms 477 --
%s
-- dir perms 500 --
%s
-- dir perms 501 --
%s
-- dir perms 502 --
%s
-- dir perms 503 --
%s
-- dir perms 504 --
%s
-- dir perms 505 --
%s
-- dir perms 506 --
%s
-- dir perms 507 --
%s
-- dir perms 510 --
%s
-- dir perms 511 --
%s
-- dir perms 512 --
%s
-- dir perms 513 --
%s
-- dir perms 514 --
%s
-- dir perms 515 --
%s
-- dir perms 516 --
%s
-- dir perms 517 --
%s
-- dir perms 520 --
%s
-- dir perms 521 --
%s
-- dir perms 522 --
%s
-- dir perms 523 --
%s
-- dir perms 524 --
%s
-- dir perms 525 --
%s
-- dir perms 526 --
%s
-- dir perms 527 --
%s
-- dir perms 530 --
%s
-- dir perms 531 --
%s
-- dir perms 532 --
%s
-- dir perms 533 --
%s
-- dir perms 534 --
%s
-- dir perms 535 --
%s
-- dir perms 536 --
%s
-- dir perms 537 --
%s
-- dir perms 540 --
%s
-- dir perms 541 --
%s
-- dir perms 542 --
%s
-- dir perms 543 --
%s
-- dir perms 544 --
%s
-- dir perms 545 --
%s
-- dir perms 546 --
%s
-- dir perms 547 --
%s
-- dir perms 550 --
%s
-- dir perms 551 --
%s
-- dir perms 552 --
%s
-- dir perms 553 --
%s
-- dir perms 554 --
%s
-- dir perms 555 --
%s
-- dir perms 556 --
%s
-- dir perms 557 --
%s
-- dir perms 560 --
%s
-- dir perms 561 --
%s
-- dir perms 562 --
%s
-- dir perms 563 --
%s
-- dir perms 564 --
%s
-- dir perms 565 --
%s
-- dir perms 566 --
%s
-- dir perms 567 --
%s
-- dir perms 570 --
%s
-- dir perms 571 --
%s
-- dir perms 572 --
%s
-- dir perms 573 --
%s
-- dir perms 574 --
%s
-- dir perms 575 --
%s
-- dir perms 576 --
%s
-- dir perms 577 --
%s
-- dir perms 600 --
%s
-- dir perms 601 --
%s
-- dir perms 602 --
%s
-- dir perms 603 --
%s
-- dir perms 604 --
%s
-- dir perms 605 --
%s
-- dir perms 606 --
%s
-- dir perms 607 --
%s
-- dir perms 610 --
%s
-- dir perms 611 --
%s
-- dir perms 612 --
%s
-- dir perms 613 --
%s
-- dir perms 614 --
%s
-- dir perms 615 --
%s
-- dir perms 616 --
%s
-- dir perms 617 --
%s
-- dir perms 620 --
%s
-- dir perms 621 --
%s
-- dir perms 622 --
%s
-- dir perms 623 --
%s
-- dir perms 624 --
%s
-- dir perms 625 --
%s
-- dir perms 626 --
%s
-- dir perms 627 --
%s
-- dir perms 630 --
%s
-- dir perms 631 --
%s
-- dir perms 632 --
%s
-- dir perms 633 --
%s
-- dir perms 634 --
%s
-- dir perms 635 --
%s
-- dir perms 636 --
%s
-- dir perms 637 --
%s
-- dir perms 640 --
%s
-- dir perms 641 --
%s
-- dir perms 642 --
%s
-- dir perms 643 --
%s
-- dir perms 644 --
%s
-- dir perms 645 --
%s
-- dir perms 646 --
%s
-- dir perms 647 --
%s
-- dir perms 650 --
%s
-- dir perms 651 --
%s
-- dir perms 652 --
%s
-- dir perms 653 --
%s
-- dir perms 654 --
%s
-- dir perms 655 --
%s
-- dir perms 656 --
%s
-- dir perms 657 --
%s
-- dir perms 660 --
%s
-- dir perms 661 --
%s
-- dir perms 662 --
%s
-- dir perms 663 --
%s
-- dir perms 664 --
%s
-- dir perms 665 --
%s
-- dir perms 666 --
%s
-- dir perms 667 --
%s
-- dir perms 670 --
%s
-- dir perms 671 --
%s
-- dir perms 672 --
%s
-- dir perms 673 --
%s
-- dir perms 674 --
%s
-- dir perms 675 --
%s
-- dir perms 676 --
%s
-- dir perms 677 --
%s
-- dir perms 700 --
%s
-- dir perms 701 --
%s
-- dir perms 702 --
%s
-- dir perms 703 --
%s
-- dir perms 704 --
%s
-- dir perms 705 --
%s
-- dir perms 706 --
%s
-- dir perms 707 --
%s
-- dir perms 710 --
%s
-- dir perms 711 --
%s
-- dir perms 712 --
%s
-- dir perms 713 --
%s
-- dir perms 714 --
%s
-- dir perms 715 --
%s
-- dir perms 716 --
%s
-- dir perms 717 --
%s
-- dir perms 720 --
%s
-- dir perms 721 --
%s
-- dir perms 722 --
%s
-- dir perms 723 --
%s
-- dir perms 724 --
%s
-- dir perms 725 --
%s
-- dir perms 726 --
%s
-- dir perms 727 --
%s
-- dir perms 730 --
%s
-- dir perms 731 --
%s
-- dir perms 732 --
%s
-- dir perms 733 --
%s
-- dir perms 734 --
%s
-- dir perms 735 --
%s
-- dir perms 736 --
%s
-- dir perms 737 --
%s
-- dir perms 740 --
%s
-- dir perms 741 --
%s
-- dir perms 742 --
%s
-- dir perms 743 --
%s
-- dir perms 744 --
%s
-- dir perms 745 --
%s
-- dir perms 746 --
%s
-- dir perms 747 --
%s
-- dir perms 750 --
%s
-- dir perms 751 --
%s
-- dir perms 752 --
%s
-- dir perms 753 --
%s
-- dir perms 754 --
%s
-- dir perms 755 --
%s
-- dir perms 756 --
%s
-- dir perms 757 --
%s
-- dir perms 760 --
%s
-- dir perms 761 --
%s
-- dir perms 762 --
%s
-- dir perms 763 --
%s
-- dir perms 764 --
%s
-- dir perms 765 --
%s
-- dir perms 766 --
%s
-- dir perms 767 --
%s
-- dir perms 770 --
%s
-- dir perms 771 --
%s
-- dir perms 772 --
%s
-- dir perms 773 --
%s
-- dir perms 774 --
%s
-- dir perms 775 --
%s
-- dir perms 776 --
%s
-- dir perms 777 --
%s
*** Done ***
