--TEST--
GH-16027 (Using SessionHandler doesn't always close session file)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$save_path = __DIR__ . '/gh16027';
@mkdir($save_path);
session_save_path($save_path);

$id = str_repeat('a1', 16);
session_id($id);
session_set_save_handler(new SessionHandler(), true);
session_start();

try {
    $_SESSION['test'] = function () {};
    session_write_close();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$file = "$save_path/sess_$id";
echo "session file exists: ";
var_dump(file_exists($file));

/* A leaked lock from close() never being called would make this fail immediately. */
$fp = fopen($file, 'r+');
echo "lock acquired after failed write: ";
var_dump(flock($fp, LOCK_EX | LOCK_NB));
fclose($fp);
?>
--CLEAN--
<?php
$save_path = __DIR__ . '/gh16027';
$id = str_repeat('a1', 16);
@unlink("$save_path/sess_$id");
@rmdir($save_path);
?>
--EXPECT--
Exception: Serialization of 'Closure' is not allowed
session file exists: bool(true)
lock acquired after failed write: bool(true)
