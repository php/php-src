--TEST--
GH-23061 (SessionHandler::create_sid() callback failure leaves the default handler open)
--EXTENSIONS--
session
--FILE--
<?php
class BrokenSidHandler extends SessionHandler {
    public function create_sid(): string {
        return [3, 6];
    }
}

class OwnStorageHandler extends SessionHandler {
    public function open(string $path, string $name): bool {
        return true;
    }
    public function read(string $id): string|false {
        return parent::read($id);
    }
    public function close(): bool {
        return true;
    }
}

session_set_save_handler(new BrokenSidHandler());
$message = '';
try {
    session_start();
} catch (Error $e) {
    $message = get_class($e) . ': ' . $e->getMessage();
}

/* The aborted session must not leave the default handler open for the next one */
session_set_save_handler(new OwnStorageHandler());
$started = session_start();
session_write_close();

echo $message, "\n";
var_dump($started);
?>
--EXPECTF--
Warning: SessionHandler::read(): Parent session handler is not open in %s on line %d

Warning: session_start(): Failed to read session data: user (path: ) in %s on line %d
Error: Session id must be a string
bool(false)
