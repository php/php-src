--TEST--
General semaphore and shared memory test
--SKIPIF--
<?php // vim600: ts=4 sw=4 syn=php fdm=marker
if(!extension_loaded('sysvsem') || !extension_loaded('sysvshm')) {
	die("skip Both sysvsem and sysvshm required");
}
?>
--FILE--
<?php
$MEMSIZE = 512;  //  size of shared memory to allocate
$SEMKEY	 =   1;  //  Semaphore key
$SHMKEY	 =   2;  //  Shared memory key

echo "Start.\n";
// Get semaphore
$sem_id = sem_get($SEMKEY, 1);
if ($sem_id === FALSE) {
	echo "Fail to get semaphore";
	exit;
}
echo "Got semaphore $sem_id.\n";

// Accuire semaphore
if (! sem_acquire($sem_id)) {
	echo "Fail to aquire semaphore $sem_id.\n";
	sem_remove($sem_id);
	exit;
}
echo "Success aquire semaphore $sem_id.\n";

$shm_id =   shm_attach($SHMKEY, $MEMSIZE);
if ($shm_id === FALSE) {
	echo "Fail to attach shared memory.\n";
	sem_remove($sem_id);
	exit;
}
echo "Success to attach shared memory : $shm_id.\n";

// Write variable 1
if (!shm_put_var($shm_id, 1, "Variable 1")) {
	echo "Fail to put var 1 on shared memory $shm_id.\n";
	sem_remove($sem_id);
	shm_remove ($shm_id);
	exit;
}
echo "Write var1 to shared memory.\n";

// Write variable 2
if (!shm_put_var($shm_id, 2, "Variable 2")) {
	echo "Fail to put var 2 on shared memory $shm_id.\n";
	sem_remove($sem_id);
	shm_remove ($shm_id);
	exit;
}
echo "Write var2 to shared memory.\n";

// Read variable 1
$var1   =   shm_get_var ($shm_id, 1);
if ($var1 === FALSE) {
	echo "Fail to retrieve Var 1 from Shared memory $shm_id, return value=$var1.\n";
} else {
	echo "Read var1=$var1.\n";
}

// Read variable 1
$var2   =   shm_get_var ($shm_id, 2);
if ($var1 === FALSE) {
	echo "Fail to retrieve Var 2 from Shared memory $shm_id, return value=$var2.\n";
} else {
	echo "Read var2=$var2.\n";
}
// Release semaphore
if (!sem_release($sem_id)) {
	echo "Fail to release $sem_id semaphore.\n";
} else {
	echo "Semaphore $sem_id released.\n";
}

// remove shared memory segmant from SysV
if (shm_remove ($shm_id)) {
	echo "Shared memory successfully removed from SysV.\n";
} else {
	echo "Fail to remove $shm_id shared memory from SysV.\n";
}

// Remove semaphore
if (sem_remove($sem_id)) {
	echo "semaphore removed successfully from SysV.\n";
} else {
	echo "Fail to remove $sem_id semaphore from SysV.\n";
}
echo "End.\n";
/* NOTE: assigned semids differ depending on the kernel, since
 *       there are actually 3 semaphores per PHP-created
 *       semaphores in effect, to keep state information.
 *       That's the reason for EXPECTF.
 */
?>
--EXPECTF--
Start.
Got semaphore Resource id #%i.
Success aquire semaphore Resource id #%i.
Success to attach shared memory : %s.
Write var1 to shared memory.
Write var2 to shared memory.
Read var1=Variable 1.
Read var2=Variable 2.
Semaphore Resource id #%s released.
Shared memory successfully removed from SysV.
semaphore removed successfully from SysV.
End.
