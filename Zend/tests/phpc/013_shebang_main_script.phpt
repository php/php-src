--TEST--
.phpc: a CLI shebang line at the top of a .phpc main script is silently skipped
--FILE--
<?php
$dir = sys_get_temp_dir();
$file = $dir . '/' . basename(__FILE__, '.php') . '_main.phpc';
file_put_contents($file,
    "#!/usr/bin/env php\necho \"shebang-skipped\\n\"; echo __LINE__, \"\\n\";");
register_shutdown_function(fn() => @unlink($file));

$php = PHP_BINARY;
$out = shell_exec(escapeshellarg($php) . ' -n ' . escapeshellarg($file));
echo $out;
?>
--EXPECT--
shebang-skipped
2
