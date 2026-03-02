--TEST--
phpversion test
--FILE--
<?php

print phpversion();
print "\n";
print phpversion('standard');
?>
--EXPECTF--
%s
%s
