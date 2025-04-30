--TEST--
Bug #61728 (PHP crash when calling ob_start in request_shutdown phase)
--EXTENSIONS--
session
--FILE--
<?php
function output_html($ext) {
    return strlen($ext);
}

class MySessionHandler implements SessionHandlerInterface {
    function open ($save_path, $session_name): bool {
        return true;
    }

    function close(): bool {
        return true;
    }

    function read ($id): string {
        return '';
    }

    function write ($id, $sess_data): bool {
        ob_start("output_html");
        echo "laruence";
        ob_end_flush();
        return true;
    }

    function destroy ($id): bool {
        return true;
    }

    function gc ($maxlifetime): int {
        return 1;
    }
}

session_set_save_handler(new MySessionHandler());
session_start();
?>
--EXPECT--
8
