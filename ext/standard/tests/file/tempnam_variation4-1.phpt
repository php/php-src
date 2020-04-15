--TEST--
Test tempnam() function: usage variations - permissions(0351 to 0777) of dir
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not valid for Windows');
}
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
/* Prototype:  string tempnam ( string $dir, string $prefix );
   Description: Create file with unique file name.
*/

/* Trying to create the file in a dir with permissions from 0351 to 0777,
     Allowable permissions: files are expected to be created in the input dir
     Non-allowable permissions: files are expected to be created in '/tmp' dir
*/

echo "*** Testing tempnam() with dir of permissions from 0351 to 0777 ***\n";
$file_path = __DIR__;
$dir_name = $file_path."/tempnam_variation4-1";
$prefix = "tempnamVar4.";

mkdir($dir_name);

for($mode = 0351; $mode <= 0777; $mode++) {
  chmod($dir_name, $mode);
  $file_name = tempnam($dir_name, $prefix);

  if(file_exists($file_name) ) {
    if (dirname($file_name) != $dir_name) {
      /* Either there's a notice or error */
       printf("%o\n", $mode);

      if (realpath(dirname($file_name)) != realpath(sys_get_temp_dir())) {
         echo " created in unexpected dir\n";
      }
    }
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
*** Testing tempnam() with dir of permissions from 0351 to 0777 ***

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
400

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
401

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
402

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
403

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
404

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
405

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
406

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
407

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
410

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
411

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
412

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
413

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
414

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
415

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
416

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
417

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
420

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
421

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
422

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
423

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
424

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
425

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
426

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
427

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
430

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
431

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
432

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
433

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
434

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
435

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
436

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
437

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
440

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
441

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
442

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
443

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
444

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
445

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
446

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
447

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
450

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
451

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
452

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
453

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
454

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
455

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
456

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
457

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
460

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
461

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
462

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
463

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
464

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
465

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
466

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
467

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
470

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
471

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
472

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
473

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
474

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
475

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
476

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
477

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
500

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
501

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
502

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
503

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
504

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
505

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
506

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
507

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
510

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
511

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
512

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
513

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
514

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
515

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
516

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
517

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
520

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
521

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
522

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
523

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
524

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
525

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
526

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
527

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
530

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
531

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
532

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
533

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
534

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
535

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
536

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
537

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
540

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
541

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
542

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
543

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
544

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
545

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
546

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
547

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
550

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
551

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
552

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
553

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
554

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
555

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
556

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
557

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
560

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
561

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
562

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
563

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
564

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
565

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
566

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
567

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
570

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
571

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
572

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
573

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
574

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
575

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
576

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
577

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
600

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
601

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
602

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
603

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
604

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
605

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
606

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
607

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
610

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
611

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
612

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
613

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
614

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
615

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
616

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
617

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
620

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
621

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
622

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
623

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
624

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
625

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
626

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
627

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
630

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
631

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
632

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
633

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
634

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
635

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
636

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
637

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
640

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
641

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
642

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
643

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
644

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
645

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
646

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
647

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
650

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
651

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
652

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
653

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
654

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
655

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
656

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
657

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
660

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
661

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
662

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
663

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
664

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
665

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
666

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
667

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
670

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
671

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
672

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
673

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
674

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
675

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
676

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation4-1.php on line 20
677
*** Done ***
