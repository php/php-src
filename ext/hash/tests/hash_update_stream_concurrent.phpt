--TEST--
hash_update_stream() under IO hooks: the context is busy while a read is suspended in it
--FILE--
<?php
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

[$r, $w] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$ctx = hash_init('sha256');

$scheduler->spawn(function () use ($ctx, $r) {
    var_dump(hash_update_stream($ctx, $r, 5));
    var_dump(hash_final($ctx) === hash('sha256', 'hello'));
});
$scheduler->spawn(function () use ($ctx, $w) {
    // The other fiber is suspended in the read: nothing may finalize, feed or copy the context
    foreach (['hash_final' => fn () => hash_final($ctx), 'hash_update' => fn () => hash_update($ctx, 'x'),
            'hash_update_file' => fn () => hash_update_file($ctx, __FILE__),
            'hash_copy' => fn () => hash_copy($ctx)] as $name => $call) {
        try {
            $call();
        } catch (Error $e) {
            echo $name, ": ", $e->getMessage(), "\n";
        }
    }
    fwrite($w, "hello");
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
?>
--EXPECT--
hash_final: Concurrent access to a HashContext
hash_update: Concurrent access to a HashContext
hash_update_file: Concurrent access to a HashContext
hash_copy: Concurrent access to a HashContext
int(5)
bool(true)
