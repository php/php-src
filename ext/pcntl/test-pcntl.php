#!../../php -q
<?

declare(ticks=1);

function alarm_handle($signal){
	 if ($signal==SIGALRM) print "Caught SIGALRM!!!\n";
}

function usr1_handle($signal){
	if ($signal==SIGUSR1) print "Caught SIGUSR1!!!\n";
}

print "This test will demonstrate a fork followed by ipc via signals.\n";

$pid=pcntl_fork();
if ($pid==0) {
	pcntl_signal(SIGUSR1, "usr1_handle");
	pcntl_signal(SIGALRM, "alarm_handle");
	print "Child: Waiting for alarm.....\n";
	sleep(100);
	print "Child: Waiting for usr1......\n";
	sleep(100);
	print "Child: Resetting Alarm handler to Ignore....\n";
	pcntl_signal(SIGALRM, SIG_IGN);
	sleep(10);
	print "Done\n";
} else {
	print "Parent: Waiting 10 seconds....\n";
	sleep(10);
	print "Parent: Sending SIGALRM to Child\n";
	posix_kill($pid,SIGALRM);
	sleep(1);
	print "Parent: Senging SIGUSR1 to Child\n";
	posix_kill($pid,SIGUSR1);
	sleep(1);
	print "Parent: Sending SIGALRM to Child\n";
	pcntl_waitpid($pid, &$status, $options);
}
