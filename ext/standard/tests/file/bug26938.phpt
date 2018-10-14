--TEST--
Bug #26938 (exec does not read consecutive long lines correctly)
--FILE--
<?php
$out = array();
$status = -1;
$php = getenv('TEST_PHP_EXECUTABLE');
if (substr(PHP_OS, 0, 3) != 'WIN') {
	exec($php . ' -n -r \''
	     . '$lengths = array(10,20000,10000,5,10000,3);'
	     . 'foreach($lengths as $length) {'
	     . '  for($i=0;$i<$length;$i++) print chr(65+$i % 27);'
	     . '  print "\n";'
	     . '}\'', $out, $status);
} else {
	exec($php . ' -n -r "'
	     . '$lengths = array(10,20000,10000,5,10000,3);'
	     . 'foreach($lengths as $length) {'
	     . '  for($i=0;$i<$length;$i++) print chr(65+$i % 27);'
	     . '  print \\"\\n\\";'
	     . '}"', $out, $status);
}
for ($i=0;$i<6;$i++)
     print "md5(line $i)= " . md5($out[$i]) . " (length " .
strlen($out[$i]) . ")\n";
?>
--EXPECT--
md5(line 0)= e86410fa2d6e2634fd8ac5f4b3afe7f3 (length 10)
md5(line 1)= e84debf3a1d132871d7fe45c1c04c566 (length 20000)
md5(line 2)= c33b4d2f86908eea5d75ee5a61fd81f4 (length 10000)
md5(line 3)= 2ecdde3959051d913f61b14579ea136d (length 5)
md5(line 4)= c33b4d2f86908eea5d75ee5a61fd81f4 (length 10000)
md5(line 5)= 902fbdd2b1df0c4f70b4a5d23525e932 (length 3)
