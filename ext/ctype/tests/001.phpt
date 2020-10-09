--TEST--
ctype on integers
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
  setlocale(LC_ALL,"C");

  function ctype_test_001($function) {
    $n=0;
    for($a=0;$a<256;$a++) {
        if($function($a)) $n++;
    }
      echo "$function $n\n";
  }
ctype_test_001("ctype_lower");
ctype_test_001("ctype_upper");
ctype_test_001("ctype_alpha");
ctype_test_001("ctype_digit");
ctype_test_001("ctype_alnum");
ctype_test_001("ctype_cntrl");
ctype_test_001("ctype_graph");
ctype_test_001("ctype_print");
ctype_test_001("ctype_punct");
ctype_test_001("ctype_space");
ctype_test_001("ctype_xdigit");
?>
--EXPECT--
ctype_lower 26
ctype_upper 26
ctype_alpha 52
ctype_digit 10
ctype_alnum 62
ctype_cntrl 33
ctype_graph 94
ctype_print 95
ctype_punct 32
ctype_space 6
ctype_xdigit 22
