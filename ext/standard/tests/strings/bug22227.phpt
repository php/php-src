--TEST--
Bug #22227 (printf() field limiters broke between 4.2.3 and 4.3.0)
--FILE--
<?php
printf("%-3.3s", "abcdef");
print "\n";
?>
--EXPECT--
abc
