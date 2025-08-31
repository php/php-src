--TEST--
Bug #47566 (return value of pcntl_wexitstatus())
--EXTENSIONS--
pcntl
--FILE--
<?php
$pid = pcntl_fork();
if ($pid == -1) {
 echo "Unable to fork";
 exit;
} elseif ($pid) {
 $epid = pcntl_waitpid(-1,$status);
 var_dump(pcntl_wexitstatus($status));
} else {
 exit(128);
}
?>
--EXPECT--
int(128)
