--TEST--
iconv test
--FILE--
<?php

$test = "Stig Sæther Bakken";
print "$test\n";
printf("%s\n", iconv("iso-8859-1", "utf-8", $test));

?>
--EXPECT--
Stig Sæther Bakken
Stig SÃ¦ther Bakken
