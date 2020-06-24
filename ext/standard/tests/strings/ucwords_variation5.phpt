--TEST--
Test ucwords() function : usage variations - custom delimiters
--FILE--
<?php
echo "*** Testing ucwords() : usage variations ***\n";

var_dump(ucwords('testing-dashed-words', '-'));
var_dump(ucwords('test(braced)words', '()'));
var_dump(ucwords('testing empty delimiters', ''));
var_dump(ucwords('testing ranges', 'a..e'));

echo "Done\n";
?>
--EXPECTF--
*** Testing ucwords() : usage variations ***
string(%d) "Testing-Dashed-Words"
string(%d) "Test(Braced)Words"
string(%d) "Testing empty delimiters"
string(%d) "TeSting raNgeS"
Done
