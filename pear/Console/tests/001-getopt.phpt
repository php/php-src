--TEST--
Console_Getopt
--SKIPIF--
skip
--FILE--
<?php

error_reporting(E_ALL);
chdir(dirname(__FILE__));
include "../Getopt.php";
PEAR::setErrorHandling(PEAR_ERROR_PRINT, "%s\n\n");

function test($argstr, $optstr) {
    $argv = split('[[:space:]]+', $argstr);
    if (PEAR::isError($options = Console_Getopt::getopt($argv, $optstr))) {
        return;
    }
    $opts = $options[0];
    $non_opts = $options[1];
    $i = 0;
    print "options: ";
    foreach ($opts as $o => $d) {
        if ($i++ > 0) {
            print ", ";
        }
        print $d[0] . '=' . $d[1];
    }
    print "\n";
    print "params: " . implode(", ", $non_opts) . "\n";
    print "\n";
}

test("-abc", "abc");
test("-abc foo", "abc");
test("-abc foo", "abc:");
test("-abc foo bar gazonk", "abc");
test("-abc foo bar gazonk", "abc:");
test("-a -b -c", "abc");
test("-a -b -c", "abc:");
test("-abc", "ab:c");
test("-abc foo -bar gazonk", "abc");
?>
--EXPECT--
options: a=, b=, c=
params: 

options: a=, b=, c=
params: foo

options: a=, b=, c=foo
params: 

options: a=, b=, c=
params: foo, bar, gazonk

options: a=, b=, c=foo
params: bar, gazonk

options: a=, b=, c=
params: 

Console_Getopt: option requires an argument -- c

options: a=, b=c
params: 

options: a=, b=, c=
params: foo, -bar, gazonk
