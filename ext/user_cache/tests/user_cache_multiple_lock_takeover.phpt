--TEST--
UserCache\Cache: batches wait for foreign locks, including after a local lease expires
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('stream_socket_pair')) {
    die('skip requires stream_socket_pair');
}
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('multiple-lock-takeover');
foreach ([false, true] as $staleLease) {
    foreach (['storeMultiple', 'deleteMultiple'] as $operation) {
        echo $staleLease ? 'stale ' : 'foreign ', $operation, "\n";
        $cache->clear();
        $cache->store('guarded', 10);
        $cache->store('other', 20);
        if ($staleLease && !$cache->lock('guarded', 1)) {
            die("initial lease failed\n");
        }
        $sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
        $pid = pcntl_fork();
        if ($pid < 0) {
            die("fork failed\n");
        }
        if ($pid === 0) {
            fclose($sockets[0]);
            if ($staleLease) {
                sleep(2);
            }
            if (!$cache->lock('guarded')) {
                exit(1);
            }
            fwrite($sockets[1], 'L');
            if (fread($sockets[1], 1) !== 'G') {
                exit(2);
            }
            usleep(300000);
            /* The unlocked first item must also remain unchanged while waiting. */
            if ($cache->fetch('guarded') !== 10 || $cache->fetch('other') !== 20) {
                exit(3);
            }
            if (!$cache->unlock('guarded')) {
                exit(4);
            }
            fclose($sockets[1]);
            exit(0);
        }
        fclose($sockets[1]);
        if (fread($sockets[0], 1) !== 'L') {
            die("lock handshake failed\n");
        }
        if ($staleLease) {
            /* Neither a default nor an equal lease can reuse a stale local token. */
            var_dump($cache->lock('guarded'));
            var_dump($cache->lock('guarded', 1));
        }
        fwrite($sockets[0], 'G');
        var_dump(match ($operation) {
            'storeMultiple' => $cache->storeMultiple(['other' => 21, 'guarded' => 11]),
            'deleteMultiple' => $cache->deleteMultiple(['other', 'guarded', 'other']),
        });
        pcntl_waitpid($pid, $status);
        fclose($sockets[0]);
        var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);
        if ($operation === 'storeMultiple') {
            var_dump($cache->fetch('guarded') === 11 && $cache->fetch('other') === 21);
        } else {
            var_dump(!$cache->has('guarded') && !$cache->has('other'));
        }
        if ($staleLease) {
            $cache->unlock('guarded');
        }
    }
}
?>
--EXPECT--
foreign storeMultiple
bool(true)
bool(true)
bool(true)
foreign deleteMultiple
bool(true)
bool(true)
bool(true)
stale storeMultiple
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
stale deleteMultiple
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
