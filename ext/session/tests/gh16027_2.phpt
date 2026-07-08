--TEST--
GH-16027 (plain SessionHandlerInterface: close() stays unreachable under a pending write() exception)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
class MySessionHandler implements SessionHandlerInterface {
    function open($save_path, $session_name): bool {
        return true;
    }

    function close(): bool {
        echo "close: goodbye cruel world\n";
        return true;
    }

    function read($id): string|false {
        return '';
    }

    function write($id, $session_data): bool {
        echo "write: goodbye cruel world\n";
        throw new Exception('write failed');
    }

    function destroy($id): bool {
        return true;
    }

    function gc($maxlifetime): int {
        return 1;
    }
}

session_set_save_handler(new MySessionHandler());
session_start();

try {
    session_write_close();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->getPrevious());
}
?>
--EXPECT--
write: goodbye cruel world
Exception: write failed
NULL
