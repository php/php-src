--TEST--
Bug #80889 (Cannot set save handler when save_handler is invalid)
--EXTENSIONS--
session
--INI--
session.save_handler=whatever
--FILE--
<?php
class DummyHandler implements SessionHandlerInterface {
    public function open($savePath, $sessionName): bool {
        return true;
    }
    public function close(): bool {
        return true;
    }
    public function read($id): string|false {
        return '';
    }
    public function write($id, $data): bool {
        return true;
    }
    public function destroy($id): bool {
        return true;
    }
    public function gc($maxlifetime): int|false {
        return true;
    }

    private int $id = 0;
	public function create_sid(): string {
	    return ++$this->id;
	}
	public function validateId(string $id): bool {
	    return $id > 0 && $id <= $this->id;
	}
}

$initHandler = ini_get('session.save_handler');
session_set_save_handler(new DummyHandler());
$setHandler = ini_get('session.save_handler');
var_dump($initHandler, $setHandler);
?>
--EXPECT--
string(8) "whatever"
string(4) "user"
