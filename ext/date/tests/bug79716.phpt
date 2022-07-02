--TEST--
Bug #79716 (Invalid date time created (with day "00"))
--FILE--
<?php
$datetime = new \DateTimeImmutable(
    '2770-01-00 15:00:00.000000',
    new \DateTimeZone('UTC')
);
\var_dump($datetime->format('j') === '0');
\var_dump($datetime);
?>
--EXPECTF--
bool(false)
object(DateTimeImmutable)#%d (%d) {
  ["date"]=>
  string(26) "2769-12-31 15:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
