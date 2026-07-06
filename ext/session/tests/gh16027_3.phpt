--TEST--
GH-16027 (exit() in close() while a write() exception is pending terminates cleanly)
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
        exit("bye\n");
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
    echo "unreachable\n";
} catch (\Throwable $e) {
    echo "unreachable catch: ", $e->getMessage(), "\n";
}
echo "unreachable after\n";
?>
--EXPECT--
write: goodbye cruel world
close: goodbye cruel world
bye
