--TEST--
Bug #42596 (session.save_path MODE option will not set "write" bit for group or world)
--EXTENSIONS--
session
--SKIPIF--
<?php
    if(substr(PHP_OS, 0, 3) == "WIN") die("skip not for Windows");
    include 'skipif.inc';
?>
--INI--
session.use_cookies=0
session.cache_limiter=
session.use_trans_sid=1
arg_separator.output="&amp;"
session.name=PHPSESSID
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php
$sessdir = __DIR__ . '/bug42596/';
mkdir($sessdir);
umask(0);
session_save_path('0;0777;' . $sessdir);
session_start();
echo "hello world\n";
session_write_close();

foreach (glob($sessdir. "*") as $sessfile) {
  var_dump(decoct(fileperms($sessfile)));
}
?>
--CLEAN--
<?php
$sessdir = __DIR__ . '/bug42596/';
array_map('unlink', glob($sessdir . '*'));
rmdir($sessdir);
?>
--EXPECT--
Deprecated: PHP Startup: Enabling session.use_trans_sid INI setting is deprecated in Unknown on line 0
hello world
string(6) "100777"
