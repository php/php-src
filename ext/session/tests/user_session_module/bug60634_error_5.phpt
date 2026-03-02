--TEST--
Bug #60634 (Segmentation fault when trying to die() in SessionHandler::write()) - fatal error in close during exec
--INI--
session.save_path=
session.name=PHPSESSID
session.save_handler=files
--EXTENSIONS--
session
--FILE--
<?php

ob_start();

class MySessionHandler implements SessionHandlerInterface {
    function open($save_path, $session_name): bool {
        return true;
    }

    function close(): bool {
        echo "close: goodbye cruel world\n";
        undefined_function();
    }

    function read($id): string {
        return '';
    }

    function write($id, $session_data): bool {
        return true;
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
session_write_close();
echo "um, hi\n";

?>
--EXPECTF--
close: goodbye cruel world

Fatal error: Uncaught Error: Call to undefined function undefined_function() in %s:%d
Stack trace:
#0 [internal function]: MySessionHandler->close()
#1 %s(%d): session_write_close()
#2 {main}
  thrown in %s on line %d
