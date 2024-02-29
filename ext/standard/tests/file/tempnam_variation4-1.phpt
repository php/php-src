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

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
400

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
401

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
402

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
403

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
404

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
405

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
406

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
407

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
410

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
411

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
412

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
413

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
414

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
415

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
416

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
417

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
420

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
421

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
422

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
423

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
424

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
425

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
426

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
427

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
430

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
431

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
432

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
433

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
434

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
435

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
436

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
437

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
440

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
441

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
442

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
443

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
444

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
445

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
446

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
447

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
450

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
451

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
452

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
453

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
454

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
455

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
456

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
457

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
460

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
461

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
462

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
463

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
464

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
465

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
466

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
467

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
470

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
471

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
472

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
473

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
474

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
475

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
476

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
477

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
500

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
501

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
502

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
503

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
504

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
505

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
506

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
507

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
510

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
511

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
512

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
513

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
514

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
515

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
516

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
517

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
520

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
521

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
522

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
523

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
524

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
525

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
526

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
527

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
530

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
531

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
532

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
533

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
534

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
535

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
536

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
537

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
540

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
541

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
542

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
543

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
544

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
545

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
546

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
547

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
550

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
551

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
552

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
553

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
554

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
555

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
556

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
557

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
560

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
561

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
562

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
563

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
564

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
565

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
566

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
567

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
570

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
571

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
572

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
573

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
574

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
575

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
576

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
577

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
600

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
601

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
602

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
603

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
604

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
605

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
606

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
607

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
610

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
611

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
612

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
613

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
614

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
615

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
616

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
617

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
620

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
621

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
622

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
623

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
624

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
625

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
626

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
627

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
630

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
631

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
632

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
633

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
634

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
635

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
636

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
637

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
640

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
641

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
642

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
643

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
644

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
645

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
646

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
647

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
650

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
651

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
652

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
653

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
654

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
655

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
656

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
657

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
660

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
661

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
662

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
663

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
664

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
665

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
666

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
667

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
670

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
671

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
672

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
673

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
674

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
675

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
676

Notice: tempnam('%s', 'tempnamVar4.'): file created in the system's temporary directory in %s on line %d
677
*** Done ***
