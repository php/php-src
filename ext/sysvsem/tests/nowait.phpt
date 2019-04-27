--TEST--
Test sem_acquire with nowait option
--SKIPIF--
<?php
if(!extension_loaded('sysvsem') || !extension_loaded('pcntl')) {
	die("skip sysvsem and pcntl required");
}
?>
--FILE--
<?php
$P_SEMKEY = ftok(__FILE__, 'P');  //  Parent Semaphore key
$C_SEMKEY = ftok(__FILE__, 'C');  //  Child Semaphore key

echo "P: parent process running.\n";

pcntl_signal(SIGCHLD, SIG_IGN);

// Get semaphore for parent
$p_sem_id = sem_get($P_SEMKEY, 1);
if ($p_sem_id === FALSE) {
	echo "P: failed to parent get semaphore\n";
	exit;
}

echo "P: got semaphore $p_sem_id.\n";

// Get semaphore for child
$c_sem_id = sem_get($C_SEMKEY, 1);
if ($c_sem_id === FALSE) {
	echo "P: failed to child get semaphore\n";
	exit;
}


// Acquire semaphore for parent
if (!sem_acquire($p_sem_id)) {
	echo "P: fail to acquire semaphore $p_sem_id.\n";
	sem_remove($p_sem_id);
	exit;
}
echo "P: acquired semaphore $p_sem_id.\n";

// Acquire semaphore for child
if (!sem_acquire($c_sem_id)) {
	echo "P: fail to acquire semaphore $c_sem_id.\n";
	sem_remove($c_sem_id);
	exit;
}
echo "P: acquired semaphore $c_sem_id\n";
// Fork process
$pid = pcntl_fork();

if ($pid) {

	register_shutdown_function(function () use ($p_sem_id) {
		echo "P: removing semaphore $p_sem_id.\n";
		sem_remove($p_sem_id);
	});

	// Release semaphore after 50ms
	usleep(50000);

	/* Wait for the child semaphore to be released to
	   to release the parent semaphore */
	if (!sem_acquire($c_sem_id)) {
		echo "P: failed to acquire semaphore $c_sem_id.\n";
		exit;
	}

	/* Release the child semahpore before releasing
	   the releasing the parent semaphore and letting
	   the child continue execution */
	sem_release($c_sem_id);

	echo "P: releasing semaphore $p_sem_id.\n";
	if (!sem_release($p_sem_id)) {
		echo "P: failed to release semaphore\n";
	}

	$status = null;
	pcntl_waitpid($pid, $status);

} else {

	register_shutdown_function(function () use ($c_sem_id) {
		echo "C: removing semaphore $c_sem_id.\n";
		sem_remove($c_sem_id);
	});

	echo "C: child process running.\n";

	// Have the semaphore after process forked
	echo "C: got semaphore $p_sem_id and $c_sem_id.\n";

	// This should fail to get to the semaphore and not wait
	if (sem_acquire($p_sem_id, true)) {
		echo "C: test failed, Child was able to acquire semaphore $p_sem_id.\n";
		exit;
	}

	// The child process did not wait to acquire the semaphore
	echo "C: failed to acquire semaphore $p_sem_id.\n";

	echo "C: releasing semaphore $c_sem_id\n";
	if (!sem_release($c_sem_id)) {
		echo "C: Failed to release semaphore\n";
	}

	// Acquire semaphore with waiting
	if (!sem_acquire($p_sem_id)) {
		echo "C: fail to acquire semaphore $p_sem_id.\n";
		exit;
	}
	echo "C: success acquired semaphore $p_sem_id.\n";

	echo "C: releasing semaphore $p_sem_id.\n";
	sem_release($p_sem_id);
}

?>
--EXPECTF--
P: parent process running.
P: got semaphore Resource id #%i.
P: acquired semaphore Resource id #%i.
P: acquired semaphore Resource id #%i
C: child process running.
C: got semaphore Resource id #%i and Resource id #%i.
C: failed to acquire semaphore Resource id #%i.
C: releasing semaphore Resource id #%i
P: releasing semaphore Resource id #%i.
C: success acquired semaphore Resource id #%i.
C: releasing semaphore Resource id #%i.
C: removing semaphore Resource id #%i.
P: removing semaphore Resource id #%i.
