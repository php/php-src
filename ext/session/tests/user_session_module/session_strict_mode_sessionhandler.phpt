--TEST--
session.use_strict_mode regenerates an unknown ID for the built-in SessionHandler
--EXTENSIONS--
session
--INI--
session.save_handler=files
session.use_strict_mode=1
session.use_cookies=0
session.cache_limiter=
session.gc_probability=0
--FILE--
<?php
$save_path = sys_get_temp_dir() . '/sess_strict_mode_sessionhandler';
@mkdir($save_path);
ini_set('session.save_path', $save_path);

$known_id = 'knownfilesessionid';
file_put_contents($save_path . '/sess_' . $known_id, 'value|s:5:"files";');

session_set_save_handler(new SessionHandler, true);

session_id($known_id);
session_start();
var_dump(session_id() === $known_id);
var_dump($_SESSION['value']);
session_write_close();

session_id('attackerchosensessionid');
session_start();
var_dump(session_id() === 'attackerchosensessionid');
session_write_close();
?>
--CLEAN--
<?php
$save_path = sys_get_temp_dir() . '/sess_strict_mode_sessionhandler';
foreach (glob($save_path . '/sess_*') as $file) {
    unlink($file);
}
rmdir($save_path);
?>
--EXPECT--
bool(true)
string(5) "files"
bool(false)
