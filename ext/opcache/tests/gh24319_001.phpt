--TEST--
GH-24319: Concurrent includes while a direct OPcache key becomes an indirect alias
--DESCRIPTION--
Cache files under their original names, invalidate them, and replace them with
symlinks to uncached files. Concurrent includes then race OPcache's conversion
of direct entries into aliases. Readers must not observe a mismatched data
pointer and indirect flag, or mistake an entry converted after lookup for a
compiled script.

This stress test disables opcache.protect_memory: the test runner normally
enables it, but repeated mprotect() calls can dominate this workload and make
its watchdog fail on slower systems. The small sequential companion test
covers protected shared-memory reads.
--EXTENSIONS--
opcache
pcntl
--SKIPIF--
<?php
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
if (PHP_OS_FAMILY === 'Windows') die('skip Unix symlinks required');
if (!function_exists('stream_socket_pair')) die('skip stream_socket_pair required');
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.jit_buffer_size=0
opcache.file_update_protection=0
opcache.validate_timestamps=0
opcache.revalidate_path=0
opcache.protect_memory=0
opcache.memory_consumption=256
opcache.interned_strings_buffer=32
opcache.max_accelerated_files=100000
opcache.max_wasted_percentage=50
--FILE--
<?php
$dir = __DIR__ . '/gh24319_001.' . getmypid();
mkdir($dir);
$numFiles = 20000;
$numReaders = 8;
$paths = [];

// All old names initially have direct entries. Invalidate them, then replace
// each file with a symlink to a different, not-yet-cached file. Loading the
// new target converts the old name to an indirect OPcache entry.
for ($i = 0; $i < $numFiles; ++$i) {
    $old = "$dir/old$i.php";
    $new = "$dir/new$i.php";
    file_put_contents($old, "<?php return 1;\n");
    file_put_contents($new, "<?php return 2;\n");
    if ((require $old) !== 1 || !opcache_invalidate($old, true)) {
        die("Warmup failed\n");
    }
    symlink($new, "$dir/link");
    rename("$dir/link", $old);
    $paths[] = $old;
}
clearstatcache(true);

$children = [];
for ($reader = 0; $reader < $numReaders; ++$reader) {
    $pair = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
    if ($pair === false || ($pid = pcntl_fork()) === -1) {
        die("Fork setup failed\n");
    }
    if ($pid === 0) {
        // Do not retain the parent's endpoints for previously forked readers.
        foreach ($children as $pipe) {
            fclose($pipe);
        }
        fclose($pair[0]);
        $start = fread($pair[1], 1);
        fclose($pair[1]);
        if ($start !== 'x') {
            exit(3);
        }
        // Keep readers on different available CPUs where supported.
        if (function_exists('pcntl_getcpuaffinity') && function_exists('pcntl_setcpuaffinity')) {
            $cpus = pcntl_getcpuaffinity();
            if ($cpus) {
                pcntl_setcpuaffinity(null, [$cpus[$reader % count($cpus)]]);
            }
        }
        pcntl_alarm(60);
        // Each entry is converted only once. Further cached passes do not
        // create additional direct-to-indirect transitions.
        foreach ($paths as $path) {
            if ((require $path) !== 2) {
                exit(2);
            }
        }
        pcntl_alarm(0);
        exit(0);
    }
    fclose($pair[1]);
    $children[$pid] = $pair[0];
}

foreach ($children as $pipe) {
    fwrite($pipe, 'x');
    fclose($pipe);
}
$failed = false;
foreach (array_keys($children) as $reader => $pid) {
    do {
        $waited = pcntl_waitpid($pid, $status);
    } while ($waited === -1 && pcntl_get_last_error() === PCNTL_EINTR);
    if ($waited !== $pid) {
        echo "Reader $reader: wait failed\n";
        $failed = true;
    } elseif (pcntl_wifsignaled($status)) {
        $signal = pcntl_wtermsig($status);
        echo "Reader $reader: signal $signal", $signal === SIGALRM ? " (watchdog)\n" : "\n";
        $failed = true;
    } elseif (!pcntl_wifexited($status) || pcntl_wexitstatus($status) !== 0) {
        echo "Reader $reader: exit ", pcntl_wexitstatus($status), "\n";
        $failed = true;
    }
}

foreach ($paths as $i => $old) {
    unlink($old);
    unlink("$dir/new$i.php");
}
rmdir($dir);
echo $failed ? "Failed\n" : "Done\n";
?>
--CLEAN--
<?php
foreach (glob(__DIR__ . '/gh24319_001.*', GLOB_ONLYDIR) ?: [] as $dir) {
    foreach (glob($dir . '/*') ?: [] as $file) {
        unlink($file);
    }
    rmdir($dir);
}
?>
--EXPECT--
Done
