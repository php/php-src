--TEST--
dirname test
--FILE--
<?php

// Allow for either Win32 or Unix.
$s = dirname("/foo");
// $s should be either / (Unix) or \ (Win32)

function check_dirname($path) {
    global $s;
    $path1 = str_replace("%",$s,$path);
    $path2 = dirname($path1);
    $path3 = str_replace($s,"%",$path2);
    print "dirname($path) == $path3\n";
}

check_dirname("%foo%");
check_dirname("%foo");
check_dirname("%foo%bar");
check_dirname("%");
check_dirname("...%foo");
check_dirname(".%foo");
check_dirname("foobar%%%");

function same($a,$b) {
    if ($a == $b) {
        print "OK\n";
    } else {
        print "FAIL  $a == $b\n";
    }
}

if ('/' == $s) {
    same(".",dirname("d:\\foo\\bar.inc"));
    same(".",dirname("c:\\foo"));
    same(".",dirname("c:\\"));
    same(".",dirname("c:"));
} else {
    same("d:\\foo",dirname("d:\\foo\\bar.inc"));
    same("c:\\",dirname("c:\\foo"));
    same("c:\\",dirname("c:\\"));
    same("c:",dirname("c:"));
}

?>
--EXPECT--
dirname(%foo%) == %
dirname(%foo) == %
dirname(%foo%bar) == %foo
dirname(%) == %
dirname(...%foo) == ...
dirname(.%foo) == .
dirname(foobar%%%) == .
OK
OK
OK
OK
