--TEST--
Closure 015: converting to string/unicode
--FILE--
<?php
$x = function() { return 1; };
print (string) $x;
print "\n";
print (unicode) $x;
print "\n";
print $x;
print "\n";
?>
--EXPECT--
Closure object
Closure object
Closure object
