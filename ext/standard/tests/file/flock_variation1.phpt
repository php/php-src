--TEST--
Test flock() function: usage variations
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. not for Windows');
}
if (function_exists("pcntl_fork") == FALSE) {
    die('skip.. no pcntl_fork()');
}
?> 
--FILE--
<?php
/* 
Prototype: bool flock(resource $handle, int $operation [, int &$wouldblock]);
Description: PHP supports a portable way of locking complete files in an advisory way
*/

echo "*** Test flock() function: with the operations given as numeric values ***\n";

$filename = dirname(__FILE__)."/flock_variation1.tmp";
$file_handle = fopen($filename, "w");
fwrite($file_handle, str_repeat("Hello2World", 10) );
fclose($file_handle);

$file_read = fopen($filename, "r");

$pid = pcntl_fork();
echo "-- child process is created --\n";
if ($pid == -1) {
  die('could not fork');
} else if ($pid) {
    echo "-- Trying to get the Reader Lock --\n";
    if( flock($file_read, LOCK_EX) ) {
      echo "-- Got Lock --\n";
      sleep(5);
      flock($file_read, LOCK_UN);
    }
    else
       echo "-- Couldn't get the Lock --\n";
} else {
    echo "-- Trying to get the Writer Lock --\n";
    if( flock($file_read, LOCK_EX) ) {
      echo "-- Got Lock --\n";
      flock($file_read, LOCK_UN);
    }
    else
       echo "-- Couldn't get the Lock --\n";

}

echo "*** Done ***\n";
?>
--CLEAN--
<?php
fclose($file_read);
fclose($file_write);
unlink($filename);
?>
--EXPECTF--	
