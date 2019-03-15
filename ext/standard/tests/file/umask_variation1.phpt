--TEST--
Test umask() function: usage variations - perms from 0000 to 0350
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
$filename = "$file_path/umask_variation1.tmp";
// temp dir used
$dirname = "$file_path/umask_variation1";

for($mask = 0000; $mask <= 0350; $mask++) {
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
-- Setting umask to 000 --
File permission : 666
Directory permission : 777
-- Setting umask to 001 --
File permission : 666
Directory permission : 776
-- Setting umask to 002 --
File permission : 664
Directory permission : 775
-- Setting umask to 003 --
File permission : 664
Directory permission : 774
-- Setting umask to 004 --
File permission : 662
Directory permission : 773
-- Setting umask to 005 --
File permission : 662
Directory permission : 772
-- Setting umask to 006 --
File permission : 660
Directory permission : 771
-- Setting umask to 007 --
File permission : 660
Directory permission : 770
-- Setting umask to 010 --
File permission : 666
Directory permission : 767
-- Setting umask to 011 --
File permission : 666
Directory permission : 766
-- Setting umask to 012 --
File permission : 664
Directory permission : 765
-- Setting umask to 013 --
File permission : 664
Directory permission : 764
-- Setting umask to 014 --
File permission : 662
Directory permission : 763
-- Setting umask to 015 --
File permission : 662
Directory permission : 762
-- Setting umask to 016 --
File permission : 660
Directory permission : 761
-- Setting umask to 017 --
File permission : 660
Directory permission : 760
-- Setting umask to 020 --
File permission : 646
Directory permission : 757
-- Setting umask to 021 --
File permission : 646
Directory permission : 756
-- Setting umask to 022 --
File permission : 644
Directory permission : 755
-- Setting umask to 023 --
File permission : 644
Directory permission : 754
-- Setting umask to 024 --
File permission : 642
Directory permission : 753
-- Setting umask to 025 --
File permission : 642
Directory permission : 752
-- Setting umask to 026 --
File permission : 640
Directory permission : 751
-- Setting umask to 027 --
File permission : 640
Directory permission : 750
-- Setting umask to 030 --
File permission : 646
Directory permission : 747
-- Setting umask to 031 --
File permission : 646
Directory permission : 746
-- Setting umask to 032 --
File permission : 644
Directory permission : 745
-- Setting umask to 033 --
File permission : 644
Directory permission : 744
-- Setting umask to 034 --
File permission : 642
Directory permission : 743
-- Setting umask to 035 --
File permission : 642
Directory permission : 742
-- Setting umask to 036 --
File permission : 640
Directory permission : 741
-- Setting umask to 037 --
File permission : 640
Directory permission : 740
-- Setting umask to 040 --
File permission : 626
Directory permission : 737
-- Setting umask to 041 --
File permission : 626
Directory permission : 736
-- Setting umask to 042 --
File permission : 624
Directory permission : 735
-- Setting umask to 043 --
File permission : 624
Directory permission : 734
-- Setting umask to 044 --
File permission : 622
Directory permission : 733
-- Setting umask to 045 --
File permission : 622
Directory permission : 732
-- Setting umask to 046 --
File permission : 620
Directory permission : 731
-- Setting umask to 047 --
File permission : 620
Directory permission : 730
-- Setting umask to 050 --
File permission : 626
Directory permission : 727
-- Setting umask to 051 --
File permission : 626
Directory permission : 726
-- Setting umask to 052 --
File permission : 624
Directory permission : 725
-- Setting umask to 053 --
File permission : 624
Directory permission : 724
-- Setting umask to 054 --
File permission : 622
Directory permission : 723
-- Setting umask to 055 --
File permission : 622
Directory permission : 722
-- Setting umask to 056 --
File permission : 620
Directory permission : 721
-- Setting umask to 057 --
File permission : 620
Directory permission : 720
-- Setting umask to 060 --
File permission : 606
Directory permission : 717
-- Setting umask to 061 --
File permission : 606
Directory permission : 716
-- Setting umask to 062 --
File permission : 604
Directory permission : 715
-- Setting umask to 063 --
File permission : 604
Directory permission : 714
-- Setting umask to 064 --
File permission : 602
Directory permission : 713
-- Setting umask to 065 --
File permission : 602
Directory permission : 712
-- Setting umask to 066 --
File permission : 600
Directory permission : 711
-- Setting umask to 067 --
File permission : 600
Directory permission : 710
-- Setting umask to 070 --
File permission : 606
Directory permission : 707
-- Setting umask to 071 --
File permission : 606
Directory permission : 706
-- Setting umask to 072 --
File permission : 604
Directory permission : 705
-- Setting umask to 073 --
File permission : 604
Directory permission : 704
-- Setting umask to 074 --
File permission : 602
Directory permission : 703
-- Setting umask to 075 --
File permission : 602
Directory permission : 702
-- Setting umask to 076 --
File permission : 600
Directory permission : 701
-- Setting umask to 077 --
File permission : 600
Directory permission : 700
-- Setting umask to 100 --
File permission : 666
Directory permission : 677
-- Setting umask to 101 --
File permission : 666
Directory permission : 676
-- Setting umask to 102 --
File permission : 664
Directory permission : 675
-- Setting umask to 103 --
File permission : 664
Directory permission : 674
-- Setting umask to 104 --
File permission : 662
Directory permission : 673
-- Setting umask to 105 --
File permission : 662
Directory permission : 672
-- Setting umask to 106 --
File permission : 660
Directory permission : 671
-- Setting umask to 107 --
File permission : 660
Directory permission : 670
-- Setting umask to 110 --
File permission : 666
Directory permission : 667
-- Setting umask to 111 --
File permission : 666
Directory permission : 666
-- Setting umask to 112 --
File permission : 664
Directory permission : 665
-- Setting umask to 113 --
File permission : 664
Directory permission : 664
-- Setting umask to 114 --
File permission : 662
Directory permission : 663
-- Setting umask to 115 --
File permission : 662
Directory permission : 662
-- Setting umask to 116 --
File permission : 660
Directory permission : 661
-- Setting umask to 117 --
File permission : 660
Directory permission : 660
-- Setting umask to 120 --
File permission : 646
Directory permission : 657
-- Setting umask to 121 --
File permission : 646
Directory permission : 656
-- Setting umask to 122 --
File permission : 644
Directory permission : 655
-- Setting umask to 123 --
File permission : 644
Directory permission : 654
-- Setting umask to 124 --
File permission : 642
Directory permission : 653
-- Setting umask to 125 --
File permission : 642
Directory permission : 652
-- Setting umask to 126 --
File permission : 640
Directory permission : 651
-- Setting umask to 127 --
File permission : 640
Directory permission : 650
-- Setting umask to 130 --
File permission : 646
Directory permission : 647
-- Setting umask to 131 --
File permission : 646
Directory permission : 646
-- Setting umask to 132 --
File permission : 644
Directory permission : 645
-- Setting umask to 133 --
File permission : 644
Directory permission : 644
-- Setting umask to 134 --
File permission : 642
Directory permission : 643
-- Setting umask to 135 --
File permission : 642
Directory permission : 642
-- Setting umask to 136 --
File permission : 640
Directory permission : 641
-- Setting umask to 137 --
File permission : 640
Directory permission : 640
-- Setting umask to 140 --
File permission : 626
Directory permission : 637
-- Setting umask to 141 --
File permission : 626
Directory permission : 636
-- Setting umask to 142 --
File permission : 624
Directory permission : 635
-- Setting umask to 143 --
File permission : 624
Directory permission : 634
-- Setting umask to 144 --
File permission : 622
Directory permission : 633
-- Setting umask to 145 --
File permission : 622
Directory permission : 632
-- Setting umask to 146 --
File permission : 620
Directory permission : 631
-- Setting umask to 147 --
File permission : 620
Directory permission : 630
-- Setting umask to 150 --
File permission : 626
Directory permission : 627
-- Setting umask to 151 --
File permission : 626
Directory permission : 626
-- Setting umask to 152 --
File permission : 624
Directory permission : 625
-- Setting umask to 153 --
File permission : 624
Directory permission : 624
-- Setting umask to 154 --
File permission : 622
Directory permission : 623
-- Setting umask to 155 --
File permission : 622
Directory permission : 622
-- Setting umask to 156 --
File permission : 620
Directory permission : 621
-- Setting umask to 157 --
File permission : 620
Directory permission : 620
-- Setting umask to 160 --
File permission : 606
Directory permission : 617
-- Setting umask to 161 --
File permission : 606
Directory permission : 616
-- Setting umask to 162 --
File permission : 604
Directory permission : 615
-- Setting umask to 163 --
File permission : 604
Directory permission : 614
-- Setting umask to 164 --
File permission : 602
Directory permission : 613
-- Setting umask to 165 --
File permission : 602
Directory permission : 612
-- Setting umask to 166 --
File permission : 600
Directory permission : 611
-- Setting umask to 167 --
File permission : 600
Directory permission : 610
-- Setting umask to 170 --
File permission : 606
Directory permission : 607
-- Setting umask to 171 --
File permission : 606
Directory permission : 606
-- Setting umask to 172 --
File permission : 604
Directory permission : 605
-- Setting umask to 173 --
File permission : 604
Directory permission : 604
-- Setting umask to 174 --
File permission : 602
Directory permission : 603
-- Setting umask to 175 --
File permission : 602
Directory permission : 602
-- Setting umask to 176 --
File permission : 600
Directory permission : 601
-- Setting umask to 177 --
File permission : 600
Directory permission : 600
-- Setting umask to 200 --
File permission : 466
Directory permission : 577
-- Setting umask to 201 --
File permission : 466
Directory permission : 576
-- Setting umask to 202 --
File permission : 464
Directory permission : 575
-- Setting umask to 203 --
File permission : 464
Directory permission : 574
-- Setting umask to 204 --
File permission : 462
Directory permission : 573
-- Setting umask to 205 --
File permission : 462
Directory permission : 572
-- Setting umask to 206 --
File permission : 460
Directory permission : 571
-- Setting umask to 207 --
File permission : 460
Directory permission : 570
-- Setting umask to 210 --
File permission : 466
Directory permission : 567
-- Setting umask to 211 --
File permission : 466
Directory permission : 566
-- Setting umask to 212 --
File permission : 464
Directory permission : 565
-- Setting umask to 213 --
File permission : 464
Directory permission : 564
-- Setting umask to 214 --
File permission : 462
Directory permission : 563
-- Setting umask to 215 --
File permission : 462
Directory permission : 562
-- Setting umask to 216 --
File permission : 460
Directory permission : 561
-- Setting umask to 217 --
File permission : 460
Directory permission : 560
-- Setting umask to 220 --
File permission : 446
Directory permission : 557
-- Setting umask to 221 --
File permission : 446
Directory permission : 556
-- Setting umask to 222 --
File permission : 444
Directory permission : 555
-- Setting umask to 223 --
File permission : 444
Directory permission : 554
-- Setting umask to 224 --
File permission : 442
Directory permission : 553
-- Setting umask to 225 --
File permission : 442
Directory permission : 552
-- Setting umask to 226 --
File permission : 440
Directory permission : 551
-- Setting umask to 227 --
File permission : 440
Directory permission : 550
-- Setting umask to 230 --
File permission : 446
Directory permission : 547
-- Setting umask to 231 --
File permission : 446
Directory permission : 546
-- Setting umask to 232 --
File permission : 444
Directory permission : 545
-- Setting umask to 233 --
File permission : 444
Directory permission : 544
-- Setting umask to 234 --
File permission : 442
Directory permission : 543
-- Setting umask to 235 --
File permission : 442
Directory permission : 542
-- Setting umask to 236 --
File permission : 440
Directory permission : 541
-- Setting umask to 237 --
File permission : 440
Directory permission : 540
-- Setting umask to 240 --
File permission : 426
Directory permission : 537
-- Setting umask to 241 --
File permission : 426
Directory permission : 536
-- Setting umask to 242 --
File permission : 424
Directory permission : 535
-- Setting umask to 243 --
File permission : 424
Directory permission : 534
-- Setting umask to 244 --
File permission : 422
Directory permission : 533
-- Setting umask to 245 --
File permission : 422
Directory permission : 532
-- Setting umask to 246 --
File permission : 420
Directory permission : 531
-- Setting umask to 247 --
File permission : 420
Directory permission : 530
-- Setting umask to 250 --
File permission : 426
Directory permission : 527
-- Setting umask to 251 --
File permission : 426
Directory permission : 526
-- Setting umask to 252 --
File permission : 424
Directory permission : 525
-- Setting umask to 253 --
File permission : 424
Directory permission : 524
-- Setting umask to 254 --
File permission : 422
Directory permission : 523
-- Setting umask to 255 --
File permission : 422
Directory permission : 522
-- Setting umask to 256 --
File permission : 420
Directory permission : 521
-- Setting umask to 257 --
File permission : 420
Directory permission : 520
-- Setting umask to 260 --
File permission : 406
Directory permission : 517
-- Setting umask to 261 --
File permission : 406
Directory permission : 516
-- Setting umask to 262 --
File permission : 404
Directory permission : 515
-- Setting umask to 263 --
File permission : 404
Directory permission : 514
-- Setting umask to 264 --
File permission : 402
Directory permission : 513
-- Setting umask to 265 --
File permission : 402
Directory permission : 512
-- Setting umask to 266 --
File permission : 400
Directory permission : 511
-- Setting umask to 267 --
File permission : 400
Directory permission : 510
-- Setting umask to 270 --
File permission : 406
Directory permission : 507
-- Setting umask to 271 --
File permission : 406
Directory permission : 506
-- Setting umask to 272 --
File permission : 404
Directory permission : 505
-- Setting umask to 273 --
File permission : 404
Directory permission : 504
-- Setting umask to 274 --
File permission : 402
Directory permission : 503
-- Setting umask to 275 --
File permission : 402
Directory permission : 502
-- Setting umask to 276 --
File permission : 400
Directory permission : 501
-- Setting umask to 277 --
File permission : 400
Directory permission : 500
-- Setting umask to 300 --
File permission : 466
Directory permission : 477
-- Setting umask to 301 --
File permission : 466
Directory permission : 476
-- Setting umask to 302 --
File permission : 464
Directory permission : 475
-- Setting umask to 303 --
File permission : 464
Directory permission : 474
-- Setting umask to 304 --
File permission : 462
Directory permission : 473
-- Setting umask to 305 --
File permission : 462
Directory permission : 472
-- Setting umask to 306 --
File permission : 460
Directory permission : 471
-- Setting umask to 307 --
File permission : 460
Directory permission : 470
-- Setting umask to 310 --
File permission : 466
Directory permission : 467
-- Setting umask to 311 --
File permission : 466
Directory permission : 466
-- Setting umask to 312 --
File permission : 464
Directory permission : 465
-- Setting umask to 313 --
File permission : 464
Directory permission : 464
-- Setting umask to 314 --
File permission : 462
Directory permission : 463
-- Setting umask to 315 --
File permission : 462
Directory permission : 462
-- Setting umask to 316 --
File permission : 460
Directory permission : 461
-- Setting umask to 317 --
File permission : 460
Directory permission : 460
-- Setting umask to 320 --
File permission : 446
Directory permission : 457
-- Setting umask to 321 --
File permission : 446
Directory permission : 456
-- Setting umask to 322 --
File permission : 444
Directory permission : 455
-- Setting umask to 323 --
File permission : 444
Directory permission : 454
-- Setting umask to 324 --
File permission : 442
Directory permission : 453
-- Setting umask to 325 --
File permission : 442
Directory permission : 452
-- Setting umask to 326 --
File permission : 440
Directory permission : 451
-- Setting umask to 327 --
File permission : 440
Directory permission : 450
-- Setting umask to 330 --
File permission : 446
Directory permission : 447
-- Setting umask to 331 --
File permission : 446
Directory permission : 446
-- Setting umask to 332 --
File permission : 444
Directory permission : 445
-- Setting umask to 333 --
File permission : 444
Directory permission : 444
-- Setting umask to 334 --
File permission : 442
Directory permission : 443
-- Setting umask to 335 --
File permission : 442
Directory permission : 442
-- Setting umask to 336 --
File permission : 440
Directory permission : 441
-- Setting umask to 337 --
File permission : 440
Directory permission : 440
-- Setting umask to 340 --
File permission : 426
Directory permission : 437
-- Setting umask to 341 --
File permission : 426
Directory permission : 436
-- Setting umask to 342 --
File permission : 424
Directory permission : 435
-- Setting umask to 343 --
File permission : 424
Directory permission : 434
-- Setting umask to 344 --
File permission : 422
Directory permission : 433
-- Setting umask to 345 --
File permission : 422
Directory permission : 432
-- Setting umask to 346 --
File permission : 420
Directory permission : 431
-- Setting umask to 347 --
File permission : 420
Directory permission : 430
-- Setting umask to 350 --
File permission : 426
Directory permission : 427
Done
