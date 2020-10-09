--TEST--
BZ2 with strings
--SKIPIF--
<?php if (!extension_loaded("bz2")) print "skip"; ?>
--FILE--
<?php

error_reporting(E_ALL);

# This FAILS
$blaat = <<<HEREDOC
This is some random data
HEREDOC;

# This Works: (so, is heredoc related)
#$blaat= 'This is some random data';

$blaat2 = bzdecompress(bzcompress($blaat));

$tests = <<<TESTS
 \$blaat === \$blaat2
TESTS;

include(__DIR__ . '/../../../tests/quicktester.inc');
?>
--EXPECT--
OK
