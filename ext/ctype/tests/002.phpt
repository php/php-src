--TEST--
ctype on strings
--SKIPIF--
<?php if (!extension_loaded("ctype")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php 
  setlocale(LC_ALL,"C"); 

  function ctype_test_002($function) {
    $n=0; $m=0;
    for($a=0;$a<256;$a++) {
    	    $c = chr($a);
	    if($function("$c$c$c")) $n++;
	    if($function("1-$c$c$c-x")) $m++;
    }
    echo "$function $n $m\n";
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
--EXPECT--
ctype_lower 26 0
ctype_upper 26 0
ctype_alpha 52 0
ctype_digit 10 0
ctype_alnum 62 0
ctype_cntrl 33 0
ctype_graph 94 94
ctype_print 95 95
ctype_punct 32 0
ctype_space 6 0
ctype_xdigit 22 0
