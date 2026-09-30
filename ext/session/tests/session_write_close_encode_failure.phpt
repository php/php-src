--TEST--
session_write_close() closes the save handler when encoding the session data fails
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php
ob_start();
$dir = __DIR__ . '/session_write_close_encode_failure';
@mkdir($dir);
session_save_path($dir);
session_start();
$file = $dir . '/sess_' . session_id();
$_SESSION['bad|key'] = 'value';
var_dump(session_write_close());

$fp = fopen($file, 'r');
var_dump(flock($fp, LOCK_EX | LOCK_NB));
fclose($fp);

session_start();
$file = $dir . '/sess_' . session_id();
$_SESSION['closure'] = function () {};
$_SESSION['bad|key'] = 'value';
try {
    session_write_close();
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

$fp = fopen($file, 'r');
var_dump(flock($fp, LOCK_EX | LOCK_NB));
fclose($fp);

class Handler extends SessionHandler
{
    public function close(): bool
    {
        echo "close\n";
        return parent::close();
    }
}

session_set_save_handler(new Handler());
session_start();
$_SESSION['bad|key'] = 'value';
var_dump(session_write_close());
echo "done\n";
?>
--CLEAN--
<?php
$dir = __DIR__ . '/session_write_close_encode_failure';
foreach (glob($dir . '/sess_*') as $file) {
    unlink($file);
}
@rmdir($dir);
?>
--EXPECTF--
Warning: session_write_close(): Failed to write session data. Data contains invalid key "bad|key" in %s on line %d
bool(true)
bool(true)

Warning: session_write_close(): Failed to write session data. Data contains invalid key "bad|key" in %s on line %d
Exception: Serialization of 'Closure' is not allowed
bool(true)

Warning: session_write_close(): Failed to write session data. Data contains invalid key "bad|key" in %s on line %d
close
bool(true)
done
