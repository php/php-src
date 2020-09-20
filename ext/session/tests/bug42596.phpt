--TEST--
Bug #42596 (session.save_path MODE option will not set "write" bit for group or world)
--SKIPIF--
<?php
    if(substr(PHP_OS, 0, 3) == "WIN") die("skip not for Windows");
    include('skipif.inc');
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
$sessdir = __DIR__.'/sessions/';
@rmdir($sessdir);
mkdir($sessdir);
$save_path = '0;0777;'.$sessdir;
umask(0);
session_save_path($save_path);
session_start();
echo "hello world\n";
session_write_close();

foreach (glob($sessdir. "*") as $sessfile) {
  var_dump(decoct(fileperms($sessfile)));
  unlink($sessfile);
}
rmdir($sessdir);
?>
--EXPECT--
hello world
string(6) "100777"
