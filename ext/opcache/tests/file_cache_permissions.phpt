--TEST--
opcache.file_cache_permissions sets the permissions of file cache files and directories
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip not for Windows');
?>
--FILE--
<?php
$dir = __DIR__ . '/file_cache_permissions';
$script = $dir . '/script.php';
mkdir($dir);
file_put_contents($script, '<?php echo "ok\n";');

function run(string $cache, string $script, array $ini): void {
    mkdir($cache);
    // New directories must keep a setgid bit they inherit (not on BSDs)
    @chmod($cache, 02700);
    mkdir($cache . '/probe');
    $setgid = fileperms($cache . '/probe') & 02000;
    rmdir($cache . '/probe');

    $cmd = [
        PHP_BINARY, '-n',
        '-d', 'opcache.enable_cli=1',
        '-d', 'opcache.file_cache="' . $cache . '"',
        '-d', 'opcache.file_cache_only=1',
        '-d', 'opcache.file_update_protection=0',
    ];
    foreach ($ini as $name => $value) {
        array_push($cmd, '-d', "$name=$value");
    }
    $cmd[] = $script;
    $proc = proc_open($cmd, [1 => ['pipe', 'w'], 2 => ['redirect', 1]], $pipes);
    echo stream_get_contents($pipes[1]);
    proc_close($proc);

    $modes = [];
    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($cache, FilesystemIterator::SKIP_DOTS),
        RecursiveIteratorIterator::SELF_FIRST
    );
    foreach ($it as $file) {
        if ($file->isDir()) {
            $modes['dir ' . decoct($file->getPerms() & 0777)] = true;
            $modes['dir setgid ' . (($file->getPerms() & 02000) === $setgid ? 'kept' : 'changed')] = true;
        } else {
            $modes['file ' . decoct($file->getPerms() & 0777)] = true;
        }
    }
    ksort($modes);
    var_dump(array_keys($modes));
}

umask(0);
run($dir . '/default', $script, []);

// The configured permissions must not be narrowed by the umask
umask(077);
run($dir . '/0640', $script, ['opcache.file_cache_permissions' => '0640']);
?>
--CLEAN--
<?php
$dir = __DIR__ . '/file_cache_permissions';
if (is_dir($dir)) {
    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir, FilesystemIterator::SKIP_DOTS),
        RecursiveIteratorIterator::CHILD_FIRST
    );
    foreach ($it as $file) {
        $file->isDir() ? rmdir($file->getPathname()) : unlink($file->getPathname());
    }
    rmdir($dir);
}
?>
--EXPECT--
ok
array(3) {
  [0]=>
  string(7) "dir 700"
  [1]=>
  string(15) "dir setgid kept"
  [2]=>
  string(8) "file 600"
}
ok
array(3) {
  [0]=>
  string(7) "dir 750"
  [1]=>
  string(15) "dir setgid kept"
  [2]=>
  string(8) "file 640"
}
