--TEST--
SPL: SplFileObject::getCurrentLine
--CREDITS--
H�vard Eide <nucleuz at gmail.com>
#Testfest php.no
--FILE--
<?php
//line 2
//line 3
//line 4
//line 5
$s = new SplFileObject(__FILE__);
$s->seek(1);
echo $s->getCurrentLine();
echo $s->getCurrentLine();
?>
--EXPECT--
//line 2
//line 3
