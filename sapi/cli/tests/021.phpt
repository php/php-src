--TEST--
CLI shell shebang
--SKIPIF--
<?php
include 'skipif.inc';
if (substr(PHP_OS, 0, 3) == 'WIN') {
	die ("skip not for Windows");
}

if (strlen("#!".getenv('TEST_PHP_EXECUTABLE')) > 127) {
        die ("skip shebang is too long, see http://www.in-ulm.de/~mascheck/various/shebang/#results");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

$filename = __DIR__.'/021.tmp.php';

$script = "#!$php -n\n".
          "ola\n".
          "<?php echo 1+1,'\n';\n".
          "?>\n".
          "adeus\n";

file_put_contents($filename, $script);
chmod($filename, 0777);

echo `$filename`;

echo "\nDone\n";
?>
--CLEAN--
<?php
unlink(__DIR__.'/021.tmp.php');
?>
--EXPECT--
ola
2
adeus

Done
