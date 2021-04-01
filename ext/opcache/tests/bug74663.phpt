--TEST--
Bug #74663 (Segfault with opcache.memory_protect and validate_timestamp)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.enable_file_override=1
opcache.validate_timestamps=1
opcache.revalidate_freq=0
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
$file = __DIR__ . "/bug74663.inc";
file_put_contents($file, "");
include $file;

var_dump(is_file($file));
?>
--CLEAN--
<?php
unlink(__DIR__ . "/bug74663.inc");
?>
--EXPECT--
bool(true)
