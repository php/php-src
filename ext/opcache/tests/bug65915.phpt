--TEST--
Bug #65915 (Inconsistent results with require return value)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
--EXTENSIONS--
opcache
--SKIPIF--
<?php
// We don't invalidate the file after the second write.
if (getenv('SKIP_REPEAT')) die("skip Not repeatable");
?>
--FILE--
<?php
$tmp = __DIR__ . "/bug65915.inc.php";

file_put_contents($tmp, '<?php return function(){ return "a";};');
$f = require $tmp;
var_dump($f());

var_dump(opcache_invalidate($tmp, true));

file_put_contents($tmp, '<?php return function(){ return "b";};');
$f = require $tmp;
var_dump($f());

@unlink($tmp);
?>
--EXPECT--
string(1) "a"
bool(true)
string(1) "b"
