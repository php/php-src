--TEST--
Bug #44667 (proc_open() does not handle pipes with the mode 'wb' correctly)
--SKIPIF--
<?php if (!is_executable('/bin/cat')) echo 'skip cat not found'; ?>
--FILE--
<?php

$pipes = array();

$descriptor_spec = array(
	0 => array('pipe', 'rb'),
	1 => array('pipe', 'wb'),
);
        
$proc = proc_open('cat', $descriptor_spec, $pipes);
        
fwrite($pipes[0], 'Hello', 5);
fflush($pipes[0]);
fclose($pipes[0]);
        
$result = fread($pipes[1], 5);
fclose($pipes[1]);
        
proc_close($proc);
        
echo "Result is: ", $result, "\n";        

echo "Done\n";

?>
--EXPECTF--
Result is: Hello
Done
