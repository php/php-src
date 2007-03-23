--TEST--
Bug #40899 (memory leak when nesting list())
--FILE--
<?php
list(list($a,$b),$c)=array(array('a','b'),'c');
echo "$a$b$c\n";
?>
--EXPECT--
abc
