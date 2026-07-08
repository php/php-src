--TEST--
GH-16027 (SessionHandler subclass throwing inside close() still releases the session file lock)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$save_path = __DIR__ . '/gh16027_3';
@mkdir($save_path);
session_save_path($save_path);

$id = str_repeat('b1', 16);
session_id($id);

class ThrowingCloseHandler extends SessionHandler {
    function close(): bool {
        throw new Exception('close blew up before delegating');
    }
}

session_set_save_handler(new ThrowingCloseHandler(), true);
session_start();

try {
    session_write_close();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$file = "$save_path/sess_$id";
echo "session file exists: ";
var_dump(file_exists($file));

/* close() threw before ever calling parent::close(); the lock must still be released. */
$fp = fopen($file, 'r+');
echo "lock acquired after close() threw: ";
var_dump(flock($fp, LOCK_EX | LOCK_NB));
fclose($fp);
?>
--CLEAN--
<?php
$save_path = __DIR__ . '/gh16027_3';
$id = str_repeat('b1', 16);
@unlink("$save_path/sess_$id");
@rmdir($save_path);
?>
--EXPECT--
Exception: close blew up before delegating
session file exists: bool(true)
lock acquired after close() threw: bool(true)
