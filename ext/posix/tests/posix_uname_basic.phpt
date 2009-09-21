--TEST--
Test posix_uname() function : basic functionality 
--SKIPIF--
<?php 
	if (!extension_loaded('posix')) die('skip - POSIX extension not loaded'); 
?>
--FILE--
<?php 
  echo "Basic test of POSIX uname function\n"; 
  	
  $uname = posix_uname();  
  unset($uname['domainname']);  
  print_r($uname);
  
?>
===DONE====
--EXPECTF--
Basic test of POSIX uname function
Array
(
    [sysname] => %s
    [nodename] => %s
    [release] => %s
    [version] => %s
    [machine] => %s
)
===DONE====
  