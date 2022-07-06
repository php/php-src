--TEST--
Bug #65768: date_diff accepts only DateTime instance even though docs say about DateTimeInterface
--INI--
date.timezone=Europe/London
--FILE--
<?php

$dt1 = new DateTime("2010-10-20");
$dti1 = new DateTimeImmutable("2010-10-25");
$dti2 = new DateTimeImmutable("2010-10-28");

$diff1 = $dt1->diff($dti1);
echo $diff1->y, " ", $diff1->m, " ", $diff1->d, " ",
     $diff1->h, " ", $diff1->i, " ", $diff1->s, "\n";

$diff2 = $dti1->diff($dti2);
echo $diff2->y, " ", $diff2->m, " ", $diff2->d, " ",
     $diff2->h, " ", $diff2->i, " ", $diff2->s, "\n";

$diff3 = date_diff($dt1, $dti2);
echo $diff3->y, " ", $diff3->m, " ", $diff3->d, " ",
     $diff3->h, " ", $diff3->i, " ", $diff3->s, "\n";

class cdt1 extends DateTime implements DateTimeInterface {}

class cdt2 extends DateTimeImmutable implements DateTimeInterface {}

class cdt3 implements DateTimeInterface {}

?>
--EXPECTF--
0 0 5 0 0 0
0 0 3 0 0 0
0 0 8 0 0 0

Fatal error: DateTimeInterface can't be implemented by user classes in %sbug65768.php on line %d
