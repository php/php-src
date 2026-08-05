--TEST--
use_strict_mode distinguishes SessionHandler from custom-storage subclasses
--EXTENSIONS--
session
--INI--
session.use_strict_mode=1
session.use_cookies=0
session.cache_limiter=
session.gc_probability=0
--FILE--
<?php
$save_path = sys_get_temp_dir() . '/sess_strict_' . getmypid();
@mkdir($save_path);
ini_set('session.save_path', $save_path);

$valid_id = 'validfilesessionid';
file_put_contents($save_path . '/sess_' . $valid_id, 'value|s:5:"files";');

session_set_save_handler(new SessionHandler, true);
session_id($valid_id);
session_start();
$session_handler_preserved = session_id() === $valid_id;
$session_handler_loaded = $_SESSION['value'] ?? null;
session_write_close();

$invalid_id = 'attackerchosensessionidyy' . bin2hex(random_bytes(4));
session_id($invalid_id);
session_start();
$session_handler_adopted = session_id() === $invalid_id;
session_write_close();

class CustomStorageHandler extends SessionHandler
{
    private array $sessions = [
        'valid-custom-session-id' => 'value|s:6:"loaded";',
    ];

    public function open(string $path, string $name): bool
    {
        return true;
    }

    public function close(): bool
    {
        return true;
    }

    public function read(string $id): string|false
    {
        return $this->sessions[$id] ?? '';
    }

    public function write(string $id, string $data): bool
    {
        $this->sessions[$id] = $data;
        return true;
    }

    public function destroy(string $id): bool
    {
        unset($this->sessions[$id]);
        return true;
    }

    public function gc(int $max_lifetime): int|false
    {
        return 0;
    }
}

session_set_save_handler(new CustomStorageHandler, true);
$id = 'valid-custom-session-id';
session_id($id);
session_start();
$custom_handler_preserved = session_id() === $id;
$custom_handler_loaded = $_SESSION['value'] ?? null;
session_write_close();

foreach (glob($save_path . '/*') as $f) {
    @unlink($f);
}
@rmdir($save_path);

echo "SessionHandler preserved: ";
var_dump($session_handler_preserved);
echo "SessionHandler loaded: ";
var_dump($session_handler_loaded);
echo "SessionHandler adopted unknown: ";
var_dump($session_handler_adopted);
echo "Custom handler preserved: ";
var_dump($custom_handler_preserved);
echo "Custom handler loaded: ";
var_dump($custom_handler_loaded);
?>
--EXPECT--
SessionHandler preserved: bool(true)
SessionHandler loaded: string(5) "files"
SessionHandler adopted unknown: bool(false)
Custom handler preserved: bool(true)
Custom handler loaded: string(6) "loaded"
