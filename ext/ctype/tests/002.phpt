--TEST--
ctype on strings
--SKIPIF--
<?php if (!extension_loaded("ctype")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php 
  setlocale("LC_ALL","C"); 

  function ctype_test_002($function) {
    $n=0; $m=0;
    for($a=0;$a<256;$a++) {
    	    $c = chr($a);
	    if($function("$c$c$c")) $n++;
	    if($function("1-$c$c$c-x")) $m++;
    }
    echo "$function $n $m\n";
  }
ctype_test_002("islower");
ctype_test_002("isupper");
ctype_test_002("isalpha");	
ctype_test_002("isdigit");	
ctype_test_002("isalnum");		
ctype_test_002("iscntrl");	
ctype_test_002("isgraph");
ctype_test_002("isprint");
ctype_test_002("ispunct");
ctype_test_002("isspace");
ctype_test_002("isxdigit");
?>
--EXPECT--
islower 26 0
isupper 26 0
isalpha 52 0
isdigit 10 0
isalnum 62 0
iscntrl 33 0
isgraph 94 94
isprint 95 95
ispunct 32 0
isspace 6 0
isxdigit 22 0
