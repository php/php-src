--TEST--
Test trim() function
--FILE--
<?php

/* second argument charlist as empty - does not trim any white spaces */
var_dump( trim("\ttesting trim", "") );

/* String with embedded NULL */
echo "\n*** Testing with String with embedded NULL ***\n";
var_dump( trim("\x0n1234\x0005678\x0000efgh\xijkl\x0n1", "\x0n1") );

/* heredoc string */
$str = <<<EOD
us
ing heredoc string
EOD;

echo "\n*** Testing with heredoc string ***\n";
var_dump( trim($str, "us\ning") );

echo "\nDone";
?>
--EXPECTF--
string(13) "	testing trim"

*** Testing with String with embedded NULL ***
string(22) "234%005678%000efgh\xijkl"

*** Testing with heredoc string ***
string(12) " heredoc str"

Done
