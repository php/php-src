--TEST--
sem_acquire with nowait
--SKIPIF--
<?php // vim600: ts=4 sw=4 syn=php fdm=marker
if(!extension_loaded('sysvsem') || !extension_loaded('pcntl')) {
	die("skip sysvsem and pcntl required");
}
?>
--FILE--
<?php
$SEMKEY	= ftok(__FILE__, 'P');  //  Semaphore key

$pid = pcntl_fork();

if ($pid) {
	echo "Parent.\n";

	pcntl_signal(SIGCHLD, SIG_IGN);

	// Get semaphore
	$sem_id = sem_get($SEMKEY, 1);
	if ($sem_id === FALSE) {
		echo "P: fail to get semaphore";
		exit;
	}
	echo "P: got semaphore $sem_id.\n";

	register_shutdown_function(function () use ($sem_id) {
		echo "P: cleanup.\n";
		sem_remove($sem_id);
	});
  
	// Acquire semaphore
	if (! sem_acquire($sem_id)) {
		echo "P: fail to acquire semaphore $sem_id.\n";
		sem_remove($sem_id);
		exit;
	}
	echo "P: success acquire semaphore $sem_id.\n";

	usleep(20000);

	echo "P: releases.\n";
	sem_release($sem_id);

	usleep(5000);

	// Acquire semaphore
	if (! sem_acquire($sem_id)) {
		echo "P: fail to acquire semaphore $sem_id.\n";
		sem_remove($sem_id);
		exit;
	}
	echo "P: success acquire semaphore $sem_id.\n";

	$status = null;
	pcntl_waitpid($pid, $status);

} else {
	usleep(10000);
	echo "Child.\n";

	// Get semaphore
	$sem_id = sem_get($SEMKEY, 1);
	if ($sem_id === FALSE) {
		echo "C: fail to get semaphore";
		exit;
	}
	echo "C: got semaphore $sem_id.\n";
  
	// Acquire semaphore
	if (! sem_acquire($sem_id)) {
		echo "C: fail to acquire semaphore $sem_id.\n";
		exit;
	}
	echo "C: success acquire semaphore $sem_id.\n";

	echo "C: releases.\n";
	sem_release($sem_id);

	usleep(10000);

	// Acquire semaphore
	if (! sem_acquire($sem_id, true)) {
		echo "C: fail to acquire semaphore $sem_id.\n";
		exit;
	}
	echo "C: success acquire semaphore $sem_id.\n";
}

?>
--EXPECTF--
Parent.
P: got semaphore Resource id #%i.
P: success acquire semaphore Resource id #%i.
Child.
C: got semaphore Resource id #%i.
P: releases.
C: success acquire semaphore Resource id #%i.
C: releases.
P: success acquire semaphore Resource id #%i.
C: fail to acquire semaphore Resource id #%i.
P: cleanup.
