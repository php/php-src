--TEST--
Phar: distinct same-length filenames in a phar archive should not collide on st_ino (phar_set_inode)
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/h13_inode_test.phar';
@unlink($fname);

$p = new Phar($fname);
$p['aaaaa.txt'] = '1';
$p['bbbbb.txt'] = '2';
$p['cccccc.txt'] = '3';
unset($p);

// Verify in a clean subprocess (-n) so PHAR_G(phar_fname_map) cache is not reused
$code = <<<'PHP'
$f = $argv[1];
$a = stat("phar://$f/aaaaa.txt")["ino"];
$b = stat("phar://$f/bbbbb.txt")["ino"];
$c = stat("phar://$f/cccccc.txt")["ino"];
var_dump($a > 0, $a !== $b, $a !== $c, $b !== $c);
PHP;

$spec = [
    0 => ['pipe', 'r'],
    1 => ['pipe', 'w'],
    2 => ['pipe', 'w'],
];
$proc = proc_open([PHP_BINARY, '-n', '-r', $code, '--', $fname], $spec, $pipes);
fclose($pipes[0]);
$stdout = stream_get_contents($pipes[1]);
$stderr = stream_get_contents($pipes[2]);
fclose($pipes[1]);
fclose($pipes[2]);
$rc = proc_close($proc);

var_dump($rc === 0, $stderr === '');
echo $stdout;
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/h13_inode_test.phar');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
