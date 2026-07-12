--TEST--
Bug GH-11310: __debugInfo does nothing on userland classes extending Date classes
--FILE--
<?php
class UDateTime extends DateTime { public function __debugInfo(): array { return ['value' => 'zzz']; } }
class UDateTimeImmutable extends DateTimeImmutable { public function __debugInfo(): array { return ['value' => 'zzz']; } }
class UDateTimeZone extends DateTimeZone { public function __debugInfo(): array { return ['value' => 'zzz']; } }
class UDateInterval extends DateInterval { public function __debugInfo(): array { return ['value' => 'zzz']; } }
class UDatePeriod extends DatePeriod { public function __debugInfo(): array { return ['value' => 'zzz']; } }

$d = new UDateTime(); var_dump($d);
$d = new UDateTimeImmutable(); var_dump($d);
$d = new UDateTimeZone("Europe/Kyiv"); var_dump($d);
$d = new UDateInterval("P3D"); var_dump($d);
$d = UDatePeriod::createFromISO8601String("2026-07-09T17:23:06Z/P3D/R5"); var_dump($d);
?>
--EXPECTF--
object(UDateTime)#%d (1) {
  ["value"]=>
  string(3) "zzz"
}
object(UDateTimeImmutable)#%d (1) {
  ["value"]=>
  string(3) "zzz"
}
object(UDateTimeZone)#%d (1) {
  ["value"]=>
  string(3) "zzz"
}
object(UDateInterval)#%d (1) {
  ["value"]=>
  string(3) "zzz"
}
object(UDatePeriod)#%d (1) {
  ["value"]=>
  string(3) "zzz"
}
