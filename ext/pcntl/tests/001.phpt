--TEST--
Test pcntl wait functionality
--SKIPIF--
<?php
	if (!extension_loaded("pcntl")) print "skip"; 
	if (!function_exists("posix_kill")) print "skip posix_kill() not available";
?>
--FILE--
<?php 
function test_exit_waits(){
	print "\n\nTesting pcntl_wifexited and wexitstatus....";

	$pid=pcntl_fork();
	if ($pid==0) {
		sleep(1);
		exit(-1);
	} else {
		$options=0;
		pcntl_waitpid($pid, $status, $options);
		if ( pcntl_wifexited($status) ) print "\nExited With: ". pcntl_wexitstatus($status);
	}
}

function test_exit_signal(){
	print "\n\nTesting pcntl_wifsignaled....";

	$pid=pcntl_fork();
    
	if ($pid==0) {
		sleep(10);
		exit;
	} else {
		$options=0;
		posix_kill($pid, SIGTERM);
		pcntl_waitpid($pid, $status, $options);
		if ( pcntl_wifsignaled($status) ) {
			$signal_print=pcntl_wtermsig($status);
			if ($signal_print==SIGTERM) $signal_print="SIGTERM";
			print "\nProcess was terminated by signal : ". $signal_print;
		}

	}
}


function test_stop_signal(){
	print "\n\nTesting pcntl_wifstopped and pcntl_wstopsig....";

	$pid=pcntl_fork();
    
	if ($pid==0) {
		sleep(1);
		exit;
	} else {
		$options=WUNTRACED;
		posix_kill($pid, SIGSTOP);
		pcntl_waitpid($pid, $status, $options);
		if ( pcntl_wifstopped($status) ) {
			$signal_print=pcntl_wstopsig($status);
			if ($signal_print==SIGSTOP) $signal_print="SIGSTOP";
			print "\nProcess was stoped by signal : ". $signal_print;
		}
		posix_kill($pid, SIGCONT);
	}
}

print "Staring wait.h tests....";
test_exit_waits();
test_exit_signal();
test_stop_signal();
?>
--EXPECT--
Staring wait.h tests....

Testing pcntl_wifexited and wexitstatus....
Exited With: -1

Testing pcntl_wifsignaled....
Process was terminated by signal : SIGTERM

Testing pcntl_wifstopped and pcntl_wstopsig....
Process was stoped by signal : SIGSTOP
