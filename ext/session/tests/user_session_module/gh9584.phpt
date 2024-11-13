--TEST--
GH-9584: PS(mod_user_class_name) must not leak into next request
--EXTENSIONS--
session
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

$handler = new MySessionHandler();
session_set_save_handler($handler);

?>
===DONE===
--EXPECT--
===DONE===
