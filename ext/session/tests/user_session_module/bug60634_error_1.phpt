--TEST--
Bug #60634 (Segmentation fault when trying to die() in SessionHandler::write()) - fatal error in write during exec
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
        return true;
    }

    function read($id): string|false {
        return '';
    }

    function write($id, $session_data): bool {
        echo "write: goodbye cruel world\n";
        undefined_function();
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

/*
FIXME: Something wrong. It should try to close after error, otherwise session
may keep "open" state.
*/

?>
--EXPECTF--
write: goodbye cruel world

Fatal error: Uncaught Error: Call to undefined function undefined_function() in %s:%d
Stack trace:
#0 [internal function]: MySessionHandler->write('%s', '')
#1 %s(%d): session_write_close()
#2 {main}
  thrown in %s on line %d
