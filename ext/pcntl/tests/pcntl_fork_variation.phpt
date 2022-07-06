--TEST--
Test function pcntl_fork() by testing the process isolation in the forking hierarchy father -> son -> grandson where father can not knows his grandson
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--SKIPIF--
<?php
    if (!extension_loaded('pcntl')) die('skip pcntl extension not available');
    elseif (!extension_loaded('posix')) die('skip posix extension not available');
?>
--FILE--
<?php
echo "*** Testing the process isolations between a process and its forks ***\n";

$pid = pcntl_fork();

if ($pid > 0) {
  pcntl_wait($status);
  echo "father is $pid\n";

  if (!isset($pid2))
  {
    echo "father ($pid) doesn't know its grandsons\n";
  }
}
else
{
  echo "son ($pid)\n";
  $pid2 = pcntl_fork();
  if ($pid2 > 0)
  {
    pcntl_wait($status2);
    echo "son is father of $pid2\n";
  }
  else
  {
    echo "grandson ($pid2)\n";
  }
}
?>
--EXPECTF--
*** Testing the process isolations between a process and its forks ***
son (0)
grandson (0)
son is father of %d
father is %d
father (%d) doesn't know its grandsons
