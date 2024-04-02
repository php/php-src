--TEST--
GH-7787: Provide more SessionHandler failure information
--EXTENSIONS--
session
--INI--
session.use_strict_mode=0
--FILE--
<?php
class MySessionHandler extends SessionHandler implements SessionUpdateTimestampHandlerInterface
{
    public function open($path, $sessname): bool {
        return true;
    }

    public function close(): bool {
        return true;
    }

    public function read($sessid): string|false {
        return 'foo|s:3:"foo";';
    }

    public function write($sessid, $sessdata): bool {
        return false;
    }

    public function destroy($sessid): bool {
        return true;
    }

    public function gc($maxlifetime): int|false {
        return true;
    }

    public function create_sid(): string {
        return sha1(random_bytes(32));
    }

    public function validateId($sid): bool {
        return true;
    }

    public function updateTimestamp($sessid, $sessdata): bool {
        return false;
    }
}

ob_start();

$handler = new MySessionHandler();
session_set_save_handler($handler);

session_start();
$_SESSION['foo'] = 'bar';
session_write_close();

session_start();
session_write_close();

session_set_save_handler(
    fn() => true,
    fn() => true,
    fn() => 'foo|s:3:"foo";',
    fn() => false,
    fn() => true,
    fn() => true,
    fn() => sha1(random_bytes(32)),
    fn() => true,
    fn() => false,
);

session_start();
$_SESSION['foo'] = 'bar';
session_write_close();

session_start();
session_write_close();

?>
--EXPECTF--
Warning: session_write_close(): Failed to write session data using user defined save handler. (session.save_path: %S, handler: MySessionHandler::write) in %s on line %d

Warning: session_write_close(): Failed to write session data using user defined save handler. (session.save_path: %S, handler: MySessionHandler::updateTimestamp) in %s on line %d

Deprecated: Calling session_set_save_handler() with more than 2 arguments is deprecated in %s on line %d

Warning: session_write_close(): Failed to write session data using user defined save handler. (session.save_path: %S, handler: write) in %s on line %d

Warning: session_write_close(): Failed to write session data using user defined save handler. (session.save_path: %S, handler: update_timestamp) in %s on line %d
