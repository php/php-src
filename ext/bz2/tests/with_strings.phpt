--TEST--
BZ2 with strings
--EXTENSIONS--
bz2
--FILE--
<?php

# This FAILS
$blaat = <<<HEREDOC
This is some random data
HEREDOC;

# This Works: (so, is heredoc related)
#$blaat= 'This is some random data';

$blaat2 = bzdecompress(bzcompress($blaat));

var_dump($blaat === $blaat2);

?>
--EXPECT--
bool(true)
