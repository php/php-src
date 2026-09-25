--TEST--
Session files: destroy removes session file even without an open descriptor
--INI--
session.use_strict_mode=0
--FILE--
<?php

$dir = __DIR__ . '/aph_ztz_sess_dir';
@mkdir($dir);

$sid = 'abcdefghijklmnopqrstuvwx01234567';
$file = $dir . '/sess_' . $sid;

@chmod($file, 0644);
@unlink($file);
touch($file);
chmod($file, 0444);

ini_set('session.save_path', $dir);
ini_set('session.save_handler', 'files');
ini_set('session.use_cookies', '0');

$done = false;
set_error_handler(function ($errno, $errstr) use (&$done, $file) {
    if (!$done && strpos($errstr, 'O_RDWR') !== false) {
        $done = true;

        // The open failure left the files module without a descriptor,
        // yet the session is still being initialized as active here.
        var_dump(session_destroy());
        var_dump(file_exists($file));
    }
    return true;
});

session_id($sid);
@session_start();

restore_error_handler();
@chmod($file, 0644);

echo "end\n";
?>
--CLEAN--
<?php
foreach (glob(__DIR__ . '/aph_ztz_sess_dir/*') as $f) {
    @chmod($f, 0644);
    @unlink($f);
}
@rmdir(__DIR__ . '/aph_ztz_sess_dir');
?>
--EXPECT--
bool(true)
bool(false)
end
