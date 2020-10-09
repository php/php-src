--TEST--
sapi_windows_set_ctrl_handler() 
--SKIPIF--
<?php

include "skipif.inc";

if (strtoupper(substr(PHP_OS, 0, 3)) !== 'WIN')
  die("skip this test is for Windows platforms only");
?>
--FILE--
<?php

$is_child = isset($argv[1]);


if ($is_child) {
    function handler($evt)
    {
        exit(3);
    }

    sapi_windows_set_ctrl_handler('handler');

    while(1) usleep(100);
} else {
    $cmd = PHP_BINARY . " -n " . $argv[0] . " 1";
    $spec = [0 => ["pipe", "r"], 1 => ["pipe", "w"]];

    $proc = proc_open($cmd, $spec, $pipes, NULL, NULL, ["bypass_shell" => true, "create_process_group" => true]);

    if (!is_resource($proc)) {
        die("Failed to start child. ");
    }

    $child_pid = proc_get_status($proc)["pid"];
    echo "Started child $child_pid\n";
    usleep(300);

    $cmd = "tasklist /FI \"PID eq $child_pid\" /NH";
    echo "Running `$cmd` to check the process indeed exists:\n";
    echo trim(shell_exec($cmd)) . "\n";

    $evt = PHP_WINDOWS_EVENT_CTRL_C;
    echo "Sending ", get_evt_name($evt), " to child $child_pid\n";
    $ret = sapi_windows_generate_ctrl_event($evt, $child_pid);

    echo ($ret ? "Successfully" : "Unsuccessfuly"), " sent ", get_evt_name($evt), " to child $child_pid\n";

    $max = 5000; $total = 0; $step = 100;
    while(proc_get_status($proc)["running"] && $max > $total) {
        usleep($step);
        $total += $step;
    }

    $status = proc_get_status($proc);
    if ($status["running"]) {
        echo "Child $child_pid didn't exit after ${max}us\n";
		foreach ($pipes as $pipe) {
			fclose($pipe);
		}
		proc_terminate($proc);
    } else {
        echo "Child $child_pid exit with status ", $status["exitcode"], " after ${total}us\n";
    }
}

function get_evt_name(int $evt) : ?string
{
    if (PHP_WINDOWS_EVENT_CTRL_C == $evt) {
        return "CTRL+C";
    } if (PHP_WINDOWS_EVENT_CTRL_BREAK == $evt) {
        return "CTRL+BREAK";
    }

    return NULL;
}

?>
--EXPECTF--
Started child %d
Running `tasklist /FI "PID eq %d" /NH` to check the process indeed exists:
php.exe%w%d%s
Sending CTRL+C to child %d
Successfully sent CTRL+C to child %d
Child %d exit with status 3 after %dus
