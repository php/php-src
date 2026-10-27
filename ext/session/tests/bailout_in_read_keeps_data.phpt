--TEST--
GH-15529: Bailout in read() does not overwrite the stored session data
--EXTENSIONS--
session
zend_test
--INI--
session.save_handler=files
session.serialize_handler=php
session.use_cookies=0
session.use_strict_mode=0
session.cache_limiter=
--FILE--
<?php
ob_start();
class Handler extends SessionHandler
{
    public function read(string $id): string|false {
        zend_trigger_bailout();
    }
    public function write(string $id, string $data): bool {
        echo "write\n";
        return parent::write($id, $data);
    }
    public function close(): bool {
        echo "close\n";
        return parent::close();
    }
}
$dir = __DIR__ . '/bailout_in_read_keeps_data';
@mkdir($dir);
$file = $dir . '/sess_gh15529read';
file_put_contents($file, 'x|i:1;');
session_save_path($dir);
session_id('gh15529read');
session_set_save_handler(new Handler(), true);
register_shutdown_function(function () use ($file) {
    var_dump(session_regenerate_id(true));
    var_dump(file_get_contents($file));
});
session_start();
echo "not reached\n";
?>
--CLEAN--
<?php
$dir = __DIR__ . '/bailout_in_read_keeps_data';
foreach (glob($dir . '/*') as $file) {
    unlink($file);
}
@rmdir($dir);
?>
--EXPECTF--
Fatal error: Bailout in %s on line %d

Warning: session_regenerate_id(): Session ID cannot be regenerated after a fatal error in session handling in %s on line %d
bool(false)
string(6) "x|i:1;"
close
