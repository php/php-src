--TEST--
Bug #71053 (Type Hint about DateTime::diff does not match with informed)
--FILE--
<?php
declare(strict_types=1);

$date1 = new \DateTime();
$date2 = clone $date1;

var_dump(count($date1->diff($date2 , true)) > 0);
?>
--EXPECT--
bool(true)
