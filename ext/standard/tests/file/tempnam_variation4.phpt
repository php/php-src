--TEST--
Test tempnam() function: usage variations - permissions(0000 to 0777) of dir
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not valid for Windows');
}
// Skip if being run by root
$filename = dirname(__FILE__)."/is_readable_root_check.tmp";
$fp = fopen($filename, 'w');
fclose($fp);
if(fileowner($filename) == 0) {
        unlink ($filename);
        die('skip cannot be run as root');
}
unlink($filename);
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
$prefix = "tempnamVar4.";

mkdir($dir_name);

for($mode = 0000; $mode <= 0777; $mode++) {
  echo "-- dir perms ";
  printf("%o", $mode);
  echo " --\n";
  chmod($dir_name, $mode);
  $file_name = tempnam($dir_name, $prefix);

  if(file_exists($file_name) ) {
    if (realpath(dirname($file_name)) == realpath(sys_get_temp_dir())) {
       $msg = " created in temp dir ";
    }
    else if (dirname($file_name) == $dir_name) {
       $msg = " created in requested dir";
    }
    else {
       $msg = " created in unexpected dir";
    }   
  
    echo $msg."\n";
    unlink($file_name);    
  }
  else {
    print("FAILED: File is not created\n");
  }
}

rmdir($dir_name);

echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing tempnam() with dir of permissions from 0000 to 0777 ***
-- dir perms 0 --
 created in temp dir 
-- dir perms 1 --
 created in temp dir 
-- dir perms 2 --
 created in temp dir 
-- dir perms 3 --
 created in temp dir 
-- dir perms 4 --
 created in temp dir 
-- dir perms 5 --
 created in temp dir 
-- dir perms 6 --
 created in temp dir 
-- dir perms 7 --
 created in temp dir 
-- dir perms 10 --
 created in temp dir 
-- dir perms 11 --
 created in temp dir 
-- dir perms 12 --
 created in temp dir 
-- dir perms 13 --
 created in temp dir 
-- dir perms 14 --
 created in temp dir 
-- dir perms 15 --
 created in temp dir 
-- dir perms 16 --
 created in temp dir 
-- dir perms 17 --
 created in temp dir 
-- dir perms 20 --
 created in temp dir 
-- dir perms 21 --
 created in temp dir 
-- dir perms 22 --
 created in temp dir 
-- dir perms 23 --
 created in temp dir 
-- dir perms 24 --
 created in temp dir 
-- dir perms 25 --
 created in temp dir 
-- dir perms 26 --
 created in temp dir 
-- dir perms 27 --
 created in temp dir 
-- dir perms 30 --
 created in temp dir 
-- dir perms 31 --
 created in temp dir 
-- dir perms 32 --
 created in temp dir 
-- dir perms 33 --
 created in temp dir 
-- dir perms 34 --
 created in temp dir 
-- dir perms 35 --
 created in temp dir 
-- dir perms 36 --
 created in temp dir 
-- dir perms 37 --
 created in temp dir 
-- dir perms 40 --
 created in temp dir 
-- dir perms 41 --
 created in temp dir 
-- dir perms 42 --
 created in temp dir 
-- dir perms 43 --
 created in temp dir 
-- dir perms 44 --
 created in temp dir 
-- dir perms 45 --
 created in temp dir 
-- dir perms 46 --
 created in temp dir 
-- dir perms 47 --
 created in temp dir 
-- dir perms 50 --
 created in temp dir 
-- dir perms 51 --
 created in temp dir 
-- dir perms 52 --
 created in temp dir 
-- dir perms 53 --
 created in temp dir 
-- dir perms 54 --
 created in temp dir 
-- dir perms 55 --
 created in temp dir 
-- dir perms 56 --
 created in temp dir 
-- dir perms 57 --
 created in temp dir 
-- dir perms 60 --
 created in temp dir 
-- dir perms 61 --
 created in temp dir 
-- dir perms 62 --
 created in temp dir 
-- dir perms 63 --
 created in temp dir 
-- dir perms 64 --
 created in temp dir 
-- dir perms 65 --
 created in temp dir 
-- dir perms 66 --
 created in temp dir 
-- dir perms 67 --
 created in temp dir 
-- dir perms 70 --
 created in temp dir 
-- dir perms 71 --
 created in temp dir 
-- dir perms 72 --
 created in temp dir 
-- dir perms 73 --
 created in temp dir 
-- dir perms 74 --
 created in temp dir 
-- dir perms 75 --
 created in temp dir 
-- dir perms 76 --
 created in temp dir 
-- dir perms 77 --
 created in temp dir 
-- dir perms 100 --
 created in temp dir 
-- dir perms 101 --
 created in temp dir 
-- dir perms 102 --
 created in temp dir 
-- dir perms 103 --
 created in temp dir 
-- dir perms 104 --
 created in temp dir 
-- dir perms 105 --
 created in temp dir 
-- dir perms 106 --
 created in temp dir 
-- dir perms 107 --
 created in temp dir 
-- dir perms 110 --
 created in temp dir 
-- dir perms 111 --
 created in temp dir 
-- dir perms 112 --
 created in temp dir 
-- dir perms 113 --
 created in temp dir 
-- dir perms 114 --
 created in temp dir 
-- dir perms 115 --
 created in temp dir 
-- dir perms 116 --
 created in temp dir 
-- dir perms 117 --
 created in temp dir 
-- dir perms 120 --
 created in temp dir 
-- dir perms 121 --
 created in temp dir 
-- dir perms 122 --
 created in temp dir 
-- dir perms 123 --
 created in temp dir 
-- dir perms 124 --
 created in temp dir 
-- dir perms 125 --
 created in temp dir 
-- dir perms 126 --
 created in temp dir 
-- dir perms 127 --
 created in temp dir 
-- dir perms 130 --
 created in temp dir 
-- dir perms 131 --
 created in temp dir 
-- dir perms 132 --
 created in temp dir 
-- dir perms 133 --
 created in temp dir 
-- dir perms 134 --
 created in temp dir 
-- dir perms 135 --
 created in temp dir 
-- dir perms 136 --
 created in temp dir 
-- dir perms 137 --
 created in temp dir 
-- dir perms 140 --
 created in temp dir 
-- dir perms 141 --
 created in temp dir 
-- dir perms 142 --
 created in temp dir 
-- dir perms 143 --
 created in temp dir 
-- dir perms 144 --
 created in temp dir 
-- dir perms 145 --
 created in temp dir 
-- dir perms 146 --
 created in temp dir 
-- dir perms 147 --
 created in temp dir 
-- dir perms 150 --
 created in temp dir 
-- dir perms 151 --
 created in temp dir 
-- dir perms 152 --
 created in temp dir 
-- dir perms 153 --
 created in temp dir 
-- dir perms 154 --
 created in temp dir 
-- dir perms 155 --
 created in temp dir 
-- dir perms 156 --
 created in temp dir 
-- dir perms 157 --
 created in temp dir 
-- dir perms 160 --
 created in temp dir 
-- dir perms 161 --
 created in temp dir 
-- dir perms 162 --
 created in temp dir 
-- dir perms 163 --
 created in temp dir 
-- dir perms 164 --
 created in temp dir 
-- dir perms 165 --
 created in temp dir 
-- dir perms 166 --
 created in temp dir 
-- dir perms 167 --
 created in temp dir 
-- dir perms 170 --
 created in temp dir 
-- dir perms 171 --
 created in temp dir 
-- dir perms 172 --
 created in temp dir 
-- dir perms 173 --
 created in temp dir 
-- dir perms 174 --
 created in temp dir 
-- dir perms 175 --
 created in temp dir 
-- dir perms 176 --
 created in temp dir 
-- dir perms 177 --
 created in temp dir 
-- dir perms 200 --
 created in temp dir 
-- dir perms 201 --
 created in temp dir 
-- dir perms 202 --
 created in temp dir 
-- dir perms 203 --
 created in temp dir 
-- dir perms 204 --
 created in temp dir 
-- dir perms 205 --
 created in temp dir 
-- dir perms 206 --
 created in temp dir 
-- dir perms 207 --
 created in temp dir 
-- dir perms 210 --
 created in temp dir 
-- dir perms 211 --
 created in temp dir 
-- dir perms 212 --
 created in temp dir 
-- dir perms 213 --
 created in temp dir 
-- dir perms 214 --
 created in temp dir 
-- dir perms 215 --
 created in temp dir 
-- dir perms 216 --
 created in temp dir 
-- dir perms 217 --
 created in temp dir 
-- dir perms 220 --
 created in temp dir 
-- dir perms 221 --
 created in temp dir 
-- dir perms 222 --
 created in temp dir 
-- dir perms 223 --
 created in temp dir 
-- dir perms 224 --
 created in temp dir 
-- dir perms 225 --
 created in temp dir 
-- dir perms 226 --
 created in temp dir 
-- dir perms 227 --
 created in temp dir 
-- dir perms 230 --
 created in temp dir 
-- dir perms 231 --
 created in temp dir 
-- dir perms 232 --
 created in temp dir 
-- dir perms 233 --
 created in temp dir 
-- dir perms 234 --
 created in temp dir 
-- dir perms 235 --
 created in temp dir 
-- dir perms 236 --
 created in temp dir 
-- dir perms 237 --
 created in temp dir 
-- dir perms 240 --
 created in temp dir 
-- dir perms 241 --
 created in temp dir 
-- dir perms 242 --
 created in temp dir 
-- dir perms 243 --
 created in temp dir 
-- dir perms 244 --
 created in temp dir 
-- dir perms 245 --
 created in temp dir 
-- dir perms 246 --
 created in temp dir 
-- dir perms 247 --
 created in temp dir 
-- dir perms 250 --
 created in temp dir 
-- dir perms 251 --
 created in temp dir 
-- dir perms 252 --
 created in temp dir 
-- dir perms 253 --
 created in temp dir 
-- dir perms 254 --
 created in temp dir 
-- dir perms 255 --
 created in temp dir 
-- dir perms 256 --
 created in temp dir 
-- dir perms 257 --
 created in temp dir 
-- dir perms 260 --
 created in temp dir 
-- dir perms 261 --
 created in temp dir 
-- dir perms 262 --
 created in temp dir 
-- dir perms 263 --
 created in temp dir 
-- dir perms 264 --
 created in temp dir 
-- dir perms 265 --
 created in temp dir 
-- dir perms 266 --
 created in temp dir 
-- dir perms 267 --
 created in temp dir 
-- dir perms 270 --
 created in temp dir 
-- dir perms 271 --
 created in temp dir 
-- dir perms 272 --
 created in temp dir 
-- dir perms 273 --
 created in temp dir 
-- dir perms 274 --
 created in temp dir 
-- dir perms 275 --
 created in temp dir 
-- dir perms 276 --
 created in temp dir 
-- dir perms 277 --
 created in temp dir 
-- dir perms 300 --
 created in requested dir
-- dir perms 301 --
 created in requested dir
-- dir perms 302 --
 created in requested dir
-- dir perms 303 --
 created in requested dir
-- dir perms 304 --
 created in requested dir
-- dir perms 305 --
 created in requested dir
-- dir perms 306 --
 created in requested dir
-- dir perms 307 --
 created in requested dir
-- dir perms 310 --
 created in requested dir
-- dir perms 311 --
 created in requested dir
-- dir perms 312 --
 created in requested dir
-- dir perms 313 --
 created in requested dir
-- dir perms 314 --
 created in requested dir
-- dir perms 315 --
 created in requested dir
-- dir perms 316 --
 created in requested dir
-- dir perms 317 --
 created in requested dir
-- dir perms 320 --
 created in requested dir
-- dir perms 321 --
 created in requested dir
-- dir perms 322 --
 created in requested dir
-- dir perms 323 --
 created in requested dir
-- dir perms 324 --
 created in requested dir
-- dir perms 325 --
 created in requested dir
-- dir perms 326 --
 created in requested dir
-- dir perms 327 --
 created in requested dir
-- dir perms 330 --
 created in requested dir
-- dir perms 331 --
 created in requested dir
-- dir perms 332 --
 created in requested dir
-- dir perms 333 --
 created in requested dir
-- dir perms 334 --
 created in requested dir
-- dir perms 335 --
 created in requested dir
-- dir perms 336 --
 created in requested dir
-- dir perms 337 --
 created in requested dir
-- dir perms 340 --
 created in requested dir
-- dir perms 341 --
 created in requested dir
-- dir perms 342 --
 created in requested dir
-- dir perms 343 --
 created in requested dir
-- dir perms 344 --
 created in requested dir
-- dir perms 345 --
 created in requested dir
-- dir perms 346 --
 created in requested dir
-- dir perms 347 --
 created in requested dir
-- dir perms 350 --
 created in requested dir
-- dir perms 351 --
 created in requested dir
-- dir perms 352 --
 created in requested dir
-- dir perms 353 --
 created in requested dir
-- dir perms 354 --
 created in requested dir
-- dir perms 355 --
 created in requested dir
-- dir perms 356 --
 created in requested dir
-- dir perms 357 --
 created in requested dir
-- dir perms 360 --
 created in requested dir
-- dir perms 361 --
 created in requested dir
-- dir perms 362 --
 created in requested dir
-- dir perms 363 --
 created in requested dir
-- dir perms 364 --
 created in requested dir
-- dir perms 365 --
 created in requested dir
-- dir perms 366 --
 created in requested dir
-- dir perms 367 --
 created in requested dir
-- dir perms 370 --
 created in requested dir
-- dir perms 371 --
 created in requested dir
-- dir perms 372 --
 created in requested dir
-- dir perms 373 --
 created in requested dir
-- dir perms 374 --
 created in requested dir
-- dir perms 375 --
 created in requested dir
-- dir perms 376 --
 created in requested dir
-- dir perms 377 --
 created in requested dir
-- dir perms 400 --
 created in temp dir 
-- dir perms 401 --
 created in temp dir 
-- dir perms 402 --
 created in temp dir 
-- dir perms 403 --
 created in temp dir 
-- dir perms 404 --
 created in temp dir 
-- dir perms 405 --
 created in temp dir 
-- dir perms 406 --
 created in temp dir 
-- dir perms 407 --
 created in temp dir 
-- dir perms 410 --
 created in temp dir 
-- dir perms 411 --
 created in temp dir 
-- dir perms 412 --
 created in temp dir 
-- dir perms 413 --
 created in temp dir 
-- dir perms 414 --
 created in temp dir 
-- dir perms 415 --
 created in temp dir 
-- dir perms 416 --
 created in temp dir 
-- dir perms 417 --
 created in temp dir 
-- dir perms 420 --
 created in temp dir 
-- dir perms 421 --
 created in temp dir 
-- dir perms 422 --
 created in temp dir 
-- dir perms 423 --
 created in temp dir 
-- dir perms 424 --
 created in temp dir 
-- dir perms 425 --
 created in temp dir 
-- dir perms 426 --
 created in temp dir 
-- dir perms 427 --
 created in temp dir 
-- dir perms 430 --
 created in temp dir 
-- dir perms 431 --
 created in temp dir 
-- dir perms 432 --
 created in temp dir 
-- dir perms 433 --
 created in temp dir 
-- dir perms 434 --
 created in temp dir 
-- dir perms 435 --
 created in temp dir 
-- dir perms 436 --
 created in temp dir 
-- dir perms 437 --
 created in temp dir 
-- dir perms 440 --
 created in temp dir 
-- dir perms 441 --
 created in temp dir 
-- dir perms 442 --
 created in temp dir 
-- dir perms 443 --
 created in temp dir 
-- dir perms 444 --
 created in temp dir 
-- dir perms 445 --
 created in temp dir 
-- dir perms 446 --
 created in temp dir 
-- dir perms 447 --
 created in temp dir 
-- dir perms 450 --
 created in temp dir 
-- dir perms 451 --
 created in temp dir 
-- dir perms 452 --
 created in temp dir 
-- dir perms 453 --
 created in temp dir 
-- dir perms 454 --
 created in temp dir 
-- dir perms 455 --
 created in temp dir 
-- dir perms 456 --
 created in temp dir 
-- dir perms 457 --
 created in temp dir 
-- dir perms 460 --
 created in temp dir 
-- dir perms 461 --
 created in temp dir 
-- dir perms 462 --
 created in temp dir 
-- dir perms 463 --
 created in temp dir 
-- dir perms 464 --
 created in temp dir 
-- dir perms 465 --
 created in temp dir 
-- dir perms 466 --
 created in temp dir 
-- dir perms 467 --
 created in temp dir 
-- dir perms 470 --
 created in temp dir 
-- dir perms 471 --
 created in temp dir 
-- dir perms 472 --
 created in temp dir 
-- dir perms 473 --
 created in temp dir 
-- dir perms 474 --
 created in temp dir 
-- dir perms 475 --
 created in temp dir 
-- dir perms 476 --
 created in temp dir 
-- dir perms 477 --
 created in temp dir 
-- dir perms 500 --
 created in temp dir 
-- dir perms 501 --
 created in temp dir 
-- dir perms 502 --
 created in temp dir 
-- dir perms 503 --
 created in temp dir 
-- dir perms 504 --
 created in temp dir 
-- dir perms 505 --
 created in temp dir 
-- dir perms 506 --
 created in temp dir 
-- dir perms 507 --
 created in temp dir 
-- dir perms 510 --
 created in temp dir 
-- dir perms 511 --
 created in temp dir 
-- dir perms 512 --
 created in temp dir 
-- dir perms 513 --
 created in temp dir 
-- dir perms 514 --
 created in temp dir 
-- dir perms 515 --
 created in temp dir 
-- dir perms 516 --
 created in temp dir 
-- dir perms 517 --
 created in temp dir 
-- dir perms 520 --
 created in temp dir 
-- dir perms 521 --
 created in temp dir 
-- dir perms 522 --
 created in temp dir 
-- dir perms 523 --
 created in temp dir 
-- dir perms 524 --
 created in temp dir 
-- dir perms 525 --
 created in temp dir 
-- dir perms 526 --
 created in temp dir 
-- dir perms 527 --
 created in temp dir 
-- dir perms 530 --
 created in temp dir 
-- dir perms 531 --
 created in temp dir 
-- dir perms 532 --
 created in temp dir 
-- dir perms 533 --
 created in temp dir 
-- dir perms 534 --
 created in temp dir 
-- dir perms 535 --
 created in temp dir 
-- dir perms 536 --
 created in temp dir 
-- dir perms 537 --
 created in temp dir 
-- dir perms 540 --
 created in temp dir 
-- dir perms 541 --
 created in temp dir 
-- dir perms 542 --
 created in temp dir 
-- dir perms 543 --
 created in temp dir 
-- dir perms 544 --
 created in temp dir 
-- dir perms 545 --
 created in temp dir 
-- dir perms 546 --
 created in temp dir 
-- dir perms 547 --
 created in temp dir 
-- dir perms 550 --
 created in temp dir 
-- dir perms 551 --
 created in temp dir 
-- dir perms 552 --
 created in temp dir 
-- dir perms 553 --
 created in temp dir 
-- dir perms 554 --
 created in temp dir 
-- dir perms 555 --
 created in temp dir 
-- dir perms 556 --
 created in temp dir 
-- dir perms 557 --
 created in temp dir 
-- dir perms 560 --
 created in temp dir 
-- dir perms 561 --
 created in temp dir 
-- dir perms 562 --
 created in temp dir 
-- dir perms 563 --
 created in temp dir 
-- dir perms 564 --
 created in temp dir 
-- dir perms 565 --
 created in temp dir 
-- dir perms 566 --
 created in temp dir 
-- dir perms 567 --
 created in temp dir 
-- dir perms 570 --
 created in temp dir 
-- dir perms 571 --
 created in temp dir 
-- dir perms 572 --
 created in temp dir 
-- dir perms 573 --
 created in temp dir 
-- dir perms 574 --
 created in temp dir 
-- dir perms 575 --
 created in temp dir 
-- dir perms 576 --
 created in temp dir 
-- dir perms 577 --
 created in temp dir 
-- dir perms 600 --
 created in temp dir 
-- dir perms 601 --
 created in temp dir 
-- dir perms 602 --
 created in temp dir 
-- dir perms 603 --
 created in temp dir 
-- dir perms 604 --
 created in temp dir 
-- dir perms 605 --
 created in temp dir 
-- dir perms 606 --
 created in temp dir 
-- dir perms 607 --
 created in temp dir 
-- dir perms 610 --
 created in temp dir 
-- dir perms 611 --
 created in temp dir 
-- dir perms 612 --
 created in temp dir 
-- dir perms 613 --
 created in temp dir 
-- dir perms 614 --
 created in temp dir 
-- dir perms 615 --
 created in temp dir 
-- dir perms 616 --
 created in temp dir 
-- dir perms 617 --
 created in temp dir 
-- dir perms 620 --
 created in temp dir 
-- dir perms 621 --
 created in temp dir 
-- dir perms 622 --
 created in temp dir 
-- dir perms 623 --
 created in temp dir 
-- dir perms 624 --
 created in temp dir 
-- dir perms 625 --
 created in temp dir 
-- dir perms 626 --
 created in temp dir 
-- dir perms 627 --
 created in temp dir 
-- dir perms 630 --
 created in temp dir 
-- dir perms 631 --
 created in temp dir 
-- dir perms 632 --
 created in temp dir 
-- dir perms 633 --
 created in temp dir 
-- dir perms 634 --
 created in temp dir 
-- dir perms 635 --
 created in temp dir 
-- dir perms 636 --
 created in temp dir 
-- dir perms 637 --
 created in temp dir 
-- dir perms 640 --
 created in temp dir 
-- dir perms 641 --
 created in temp dir 
-- dir perms 642 --
 created in temp dir 
-- dir perms 643 --
 created in temp dir 
-- dir perms 644 --
 created in temp dir 
-- dir perms 645 --
 created in temp dir 
-- dir perms 646 --
 created in temp dir 
-- dir perms 647 --
 created in temp dir 
-- dir perms 650 --
 created in temp dir 
-- dir perms 651 --
 created in temp dir 
-- dir perms 652 --
 created in temp dir 
-- dir perms 653 --
 created in temp dir 
-- dir perms 654 --
 created in temp dir 
-- dir perms 655 --
 created in temp dir 
-- dir perms 656 --
 created in temp dir 
-- dir perms 657 --
 created in temp dir 
-- dir perms 660 --
 created in temp dir 
-- dir perms 661 --
 created in temp dir 
-- dir perms 662 --
 created in temp dir 
-- dir perms 663 --
 created in temp dir 
-- dir perms 664 --
 created in temp dir 
-- dir perms 665 --
 created in temp dir 
-- dir perms 666 --
 created in temp dir 
-- dir perms 667 --
 created in temp dir 
-- dir perms 670 --
 created in temp dir 
-- dir perms 671 --
 created in temp dir 
-- dir perms 672 --
 created in temp dir 
-- dir perms 673 --
 created in temp dir 
-- dir perms 674 --
 created in temp dir 
-- dir perms 675 --
 created in temp dir 
-- dir perms 676 --
 created in temp dir 
-- dir perms 677 --
 created in temp dir 
-- dir perms 700 --
 created in requested dir
-- dir perms 701 --
 created in requested dir
-- dir perms 702 --
 created in requested dir
-- dir perms 703 --
 created in requested dir
-- dir perms 704 --
 created in requested dir
-- dir perms 705 --
 created in requested dir
-- dir perms 706 --
 created in requested dir
-- dir perms 707 --
 created in requested dir
-- dir perms 710 --
 created in requested dir
-- dir perms 711 --
 created in requested dir
-- dir perms 712 --
 created in requested dir
-- dir perms 713 --
 created in requested dir
-- dir perms 714 --
 created in requested dir
-- dir perms 715 --
 created in requested dir
-- dir perms 716 --
 created in requested dir
-- dir perms 717 --
 created in requested dir
-- dir perms 720 --
 created in requested dir
-- dir perms 721 --
 created in requested dir
-- dir perms 722 --
 created in requested dir
-- dir perms 723 --
 created in requested dir
-- dir perms 724 --
 created in requested dir
-- dir perms 725 --
 created in requested dir
-- dir perms 726 --
 created in requested dir
-- dir perms 727 --
 created in requested dir
-- dir perms 730 --
 created in requested dir
-- dir perms 731 --
 created in requested dir
-- dir perms 732 --
 created in requested dir
-- dir perms 733 --
 created in requested dir
-- dir perms 734 --
 created in requested dir
-- dir perms 735 --
 created in requested dir
-- dir perms 736 --
 created in requested dir
-- dir perms 737 --
 created in requested dir
-- dir perms 740 --
 created in requested dir
-- dir perms 741 --
 created in requested dir
-- dir perms 742 --
 created in requested dir
-- dir perms 743 --
 created in requested dir
-- dir perms 744 --
 created in requested dir
-- dir perms 745 --
 created in requested dir
-- dir perms 746 --
 created in requested dir
-- dir perms 747 --
 created in requested dir
-- dir perms 750 --
 created in requested dir
-- dir perms 751 --
 created in requested dir
-- dir perms 752 --
 created in requested dir
-- dir perms 753 --
 created in requested dir
-- dir perms 754 --
 created in requested dir
-- dir perms 755 --
 created in requested dir
-- dir perms 756 --
 created in requested dir
-- dir perms 757 --
 created in requested dir
-- dir perms 760 --
 created in requested dir
-- dir perms 761 --
 created in requested dir
-- dir perms 762 --
 created in requested dir
-- dir perms 763 --
 created in requested dir
-- dir perms 764 --
 created in requested dir
-- dir perms 765 --
 created in requested dir
-- dir perms 766 --
 created in requested dir
-- dir perms 767 --
 created in requested dir
-- dir perms 770 --
 created in requested dir
-- dir perms 771 --
 created in requested dir
-- dir perms 772 --
 created in requested dir
-- dir perms 773 --
 created in requested dir
-- dir perms 774 --
 created in requested dir
-- dir perms 775 --
 created in requested dir
-- dir perms 776 --
 created in requested dir
-- dir perms 777 --
 created in requested dir
*** Done ***
