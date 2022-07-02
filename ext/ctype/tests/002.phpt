--TEST--
ctype on strings
--EXTENSIONS--
ctype
--FILE--
<?php

setlocale(LC_ALL,"C");
print "LOCALE is '" . setlocale(LC_ALL,0) . "'\n";

function ctype_test_002($function) {
    $n1 = $n2 = $n3 = 0;
    // test portable POSIX characters 0..127
    for ($a=0;$a<128;$a++) {
        $c = chr($a);
        if($function($c)) $n1++;
        if($function("$c$c$c")) $n2++;
        if($function("1-$c$c$c-x")) $n3++;
    }
    print "$function $n1 $n2 $n3\n";
}

ctype_test_002("ctype_lower");
ctype_test_002("ctype_upper");
ctype_test_002("ctype_alpha");
ctype_test_002("ctype_digit");
ctype_test_002("ctype_alnum");
ctype_test_002("ctype_cntrl");
ctype_test_002("ctype_graph");
ctype_test_002("ctype_print");
ctype_test_002("ctype_punct");
ctype_test_002("ctype_space");
ctype_test_002("ctype_xdigit");

?>
--EXPECTF--
LOCALE is '%s'
ctype_lower 26 26 0
ctype_upper 26 26 0
ctype_alpha 52 52 0
ctype_digit 10 10 0
ctype_alnum 62 62 0
ctype_cntrl 33 33 0
ctype_graph 94 94 94
ctype_print 95 95 95
ctype_punct 32 32 0
ctype_space 6 6 0
ctype_xdigit 22 22 0
