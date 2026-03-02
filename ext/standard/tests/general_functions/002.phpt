--TEST--
quoted_printable_decode() function test
--FILE--
<?php echo quoted_printable_decode("=FAwow-factor=C1=d0=D5=DD=C5=CE=CE=D9=C5=0A=
=20=D4=cf=D2=C7=CF=D7=D9=C5=
=20=
=D0=
=D2=CF=C5=CB=D4=D9"); ?>
--EXPECT--
Зwow-factorапущенные
 торговые проекты
