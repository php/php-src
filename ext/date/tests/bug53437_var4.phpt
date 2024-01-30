--TEST--
Bug #53437 (Check that var_dump out is the same using the whole object or it's single properties), variation 4
--INI--
date.timezone = Europe/Berlin
--FILE--
<?php
$dt = new DateTime('2009-10-11');

$df = $dt->diff(new DateTime('2009-10-13'));

var_dump($df,
    $df->y,
    $df->m,
    $df->d,
    $df->h,
    $df->i,
    $df->s,
    $df->f,
    $df->invert,
    $df->days);

?>
--EXPECTF--
object(DateTimeInterval)#%d (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(2)
  ["h"]=>
  int(0)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(2)
}
int(0)
int(0)
int(2)
int(0)
int(0)
int(0)
float(0)
int(0)
int(2)
