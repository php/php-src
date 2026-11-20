<?php
function preload_child_sigblk(): string {
    $proc = proc_open(['cat', '/proc/self/status'], [1 => ['pipe', 'w']], $pipes);
    preg_match('/^SigBlk:\s*(\S+)/m', stream_get_contents($pipes[1]), $m);
    proc_close($proc);
    return $m[1];
}

file_put_contents(__DIR__ . '/preload_signal_mask.txt', preload_child_sigblk());
