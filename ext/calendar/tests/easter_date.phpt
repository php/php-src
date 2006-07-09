--TEST--
easter_date()
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
echo date("Y-m-d", easter_date(2000))."\n";       
echo date("Y-m-d", easter_date(2001))."\n";      
echo date("Y-m-d", easter_date(2002))."\n";      
?>
--EXPECT--
2000-04-23
2001-04-15
2002-03-31
