--TEST--
Bug #64653 (Subtraction of DateInterval yields wrong result)
--FILE--
<?php
$date = new \DateTime('2370-01-31');
echo $date->sub(new \DateInterval('P1M'))->format('Y-m-d');
?>
--EXPECT--
2369-12-31
