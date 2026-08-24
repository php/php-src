--TEST--
GH-15529: Bailout while encoding session data still closes a SessionHandler-based handler
--EXTENSIONS--
session
zend_test
--INI--
session.save_handler=files
session.serialize_handler=php
--FILE--
<?php
ob_start();
class Handler extends SessionHandler
{
    public function write(string $id, string $data): bool {
        echo "write\n";
        return parent::write($id, $data);
    }
    public function close(): bool {
        echo "close\n";
        return parent::close();
    }
}
class Bomb
{
    public function __serialize(): array {
        zend_trigger_bailout();
    }
}
$dir = __DIR__ . '/bailout_in_encode_closes_handler';
@mkdir($dir);
session_save_path($dir);
session_set_save_handler(new Handler(), true);
session_start();
$_SESSION['bomb'] = new Bomb();
session_write_close();
echo "not reached\n";
?>
--CLEAN--
<?php
$dir = __DIR__ . '/bailout_in_encode_closes_handler';
foreach (glob($dir . '/*') as $file) {
    unlink($file);
}
@rmdir($dir);
?>
--EXPECTF--
Fatal error: Bailout in %s on line %d
close
