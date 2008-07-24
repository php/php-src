--TEST--
Phar: phar.cache_list basic read test
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.cache_list={$cwd}/files/nophar.phar
--FILE--
<?php
$pname = 'phar://' . dirname(__FILE__) . '/files/nophar.phar';
var_dump(file_get_contents($pname . '/b/c.php'));
$a = opendir($pname);
while (false !== ($b = readdir($a))) {
var_dump($b);
}
foreach (new RecursiveIteratorIterator(new Phar($pname)) as $f) {
	var_dump($f->getPathName());
}
var_dump(is_dir($pname . '/b'));
var_dump(is_dir($pname . '/d'));
var_dump(is_dir($pname . '/b/c.php'));
?>
===DONE===
--EXPECTF--
string(131) "<?php echo "in b\n";$a = fopen("index.php", "r", true);echo stream_get_contents($a);fclose($a);include dirname(__FILE__) . "/../d";"
string(1) "b"
string(1) "d"
string(9) "index.php"
string(7) "web.php"
string(%d) "phar://%snophar.phar/b/c.php"
string(%d) "phar://%snophar.phar/d"
string(%d) "phar://%snophar.phar/index.php"
string(%d) "phar://%snophar.phar/web.php"
bool(true)
bool(false)
bool(false)
===DONE===
