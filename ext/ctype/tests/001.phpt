--TEST--
ctype on integers
--SKIPIF--
<?php if (!extension_loaded("ctype")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php 
  setlocale("LC_ALL","C"); 

  function ctype_test_001($function) {
    $n=0; 	
    for($a=0;$a<256;$a++) {
	    if($function($a)) $n++;
    }
	  echo "$function $n\n";
  }
ctype_test_001("islower");
ctype_test_001("isupper");
ctype_test_001("isalpha");	
ctype_test_001("isdigit");	
ctype_test_001("isalnum");		
ctype_test_001("iscntrl");	
ctype_test_001("isgraph");
ctype_test_001("isprint");
ctype_test_001("ispunct");
ctype_test_001("isspace");
ctype_test_001("isxdigit");
?>
--EXPECT--
islower 26
isupper 26
isalpha 52
isdigit 10
isalnum 62
iscntrl 33
isgraph 94
isprint 95
ispunct 32
isspace 6
isxdigit 22
