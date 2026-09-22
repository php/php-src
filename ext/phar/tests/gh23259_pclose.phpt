--TEST--
GH-23259 (Stream-error finalization uses streams after close) - pclose()
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$directory = __DIR__ . '/gh23259_pclose';
$archive = $directory . '/close-error.phar';

mkdir($directory);
$phar = new Phar($archive);
$phar['seed.txt'] = 'seed';
unset($phar);

$context = stream_context_create(['stream' => ['error_mode' => StreamErrorMode::Error]]);
$stream = fopen("phar://$archive/new.txt", 'wb', false, $context);
unset($context);
fwrite($stream, str_repeat('A', 4096));

unlink($archive);

var_dump(pclose($stream));
?>
--CLEAN--
<?php
$directory = __DIR__ . '/gh23259_pclose';
@unlink($directory . '/close-error.phar');
@rmdir($directory);
?>
--EXPECTF--
Warning: pclose(): unable to seek to start of file "seed.txt" while creating new phar "%s" in %s on line %d

Warning: pclose(): unable to seek to start of file "seed.txt" while creating new phar "%s" in %s on line %d
int(-1)
