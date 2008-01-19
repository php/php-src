--TEST--
Phar::setStub()/stopBuffering()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$p = new Phar(dirname(__FILE__) . '/brandnewphar.phar', 0, 'brandnewphar.phar');
$p['file1.txt'] = 'hi';
$p->stopBuffering();
var_dump($p->getStub());
$p->setStub("<?php
function __autoload(\$class)
{
    include 'phar://' . str_replace('_', '/', \$class);
}
Phar::mapPhar('brandnewphar.phar');
include 'phar://brandnewphar.phar/startup.php';
__HALT_COMPILER();
?>");
var_dump($p->getStub());
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/brandnewphar.phar');
__HALT_COMPILER();
?>
--EXPECT--
string(4501) "<?php
if (in_array('phar', stream_get_wrappers()) && class_exists('Phar', 0)) {
Phar::interceptFileFuncs();
include 'phar://' . __FILE__ . '/' . Extract_Phar::START;
return;
}
class Extract_Phar
{
static $tmp = array();
static $origdir;
const GZ = 0x1000;
const BZ2 = 0x2000;
const MASK = 0x3000;
const START = 'index.php';
static function go($D)
{
register_shutdown_function(array('Extract_Phar', '_removeTmpFiles'));
$fp = fopen(__FILE__, 'rb');
fseek($fp, $D);
$L = unpack('V', fread($fp, 4));
$m = '';
do {
$read = 8192;
if ($L[1] - strlen($m) < 8192) {
$read = $L[1] - strlen($m);
}
$last = fread($fp, $read);
$m .= $last;
} while (strlen($last) && strlen($m) < $L[1]);
if (strlen($m) < $L[1]) {
die('ERROR: manifest length read was "' .
strlen($m) .'" should be "' .
$L[1] . '"');
}
$info = self::_unpack($m);
$f = $info['c'];
if ($f & self::GZ) {
if (!function_exists('gzinflate')) {
die('Error: zlib extension is not enabled - gzinflate() function needed' .
' for compressed .phars');
}
}
if ($f & self::BZ2) {
if (!function_exists('bzdecompress')) {
die('Error: bzip2 extension is not enabled - bzdecompress() function needed' .
' for compressed .phars');
}
}
$temp = self::tmpdir();
if (!$temp) {
$sessionpath = session_save_path();
if (strpos ($sessionpath, ";") !== FALSE)
$sessionpath = substr ($sessionpath, strpos ($sessionpath, ";")+1);
if (!file_exists($sessionpath) && !is_dir($sessionpath)) {
die('Could not locate temporary directory to extract phar');
}
$temp = $sessionpath;
}
$temp .= '/pharextract';
if (!file_exists($temp)) {
@mkdir($temp);
$temp = realpath($temp);
self::$tmp[] = $temp;
}
self::$origdir = getcwd();
foreach ($info['m'] as $path => $file) {
$a = !file_exists(dirname($temp . '/' . $path));
@mkdir(dirname($temp . '/' . $path), 0777, true);
clearstatcache();
if ($a) self::$tmp[] = realpath(dirname($temp . '/' . $path));
if ($path[strlen($path) - 1] == '/') {
mkdir($temp . '/' . $path);
} else {
file_put_contents($temp . '/' . $path, self::extractFile($path, $file, $fp));
}
self::$tmp[] = realpath($temp . '/' . $path);
}
chdir($temp);
include self::START;
}

static function tmpdir()
{
if (strpos(PHP_OS, 'WIN') !== false) {
if ($var = isset($_ENV['TMP']) ? $_ENV['TMP'] : getenv('TMP')) {
return $var;
}
if ($var = isset($_ENV['TEMP']) ? $_ENV['TEMP'] : getenv('TEMP')) {
return $var;
}
if ($var = isset($_ENV['USERPROFILE']) ? $_ENV['USERPROFILE'] : @getenv('USERPROFILE')) {
return $var;
}
if ($var = isset($_ENV['windir']) ? $_ENV['windir'] : getenv('windir')) {
return $var;
}
return @getenv('SystemRoot') . '	emp';
}
if ($var = isset($_ENV['TMPDIR']) ? $_ENV['TMPDIR'] : getenv('TMPDIR')) {
return $var;
}
return realpath('/tmp');
}

static function _unpack($m)
{
$info = unpack('V', substr($m, 0, 4));
// skip API version, phar flags, alias, metadata
$l = unpack('V', substr($m, 10, 4));
$m = substr($m, 14 + $l[1]);
$s = unpack('V', substr($m, 0, 4));
$o = 0;
$start = 4 + $s[1];
$ret['c'] = 0;
for ($i = 0; $i < $info[1]; $i++) {
// length of the file name
$len = unpack('V', substr($m, $start, 4));
$start += 4;
// file name
$savepath = substr($m, $start, $len[1]);
$start += $len[1];
// retrieve manifest data:
// 0 = size, 1 = timestamp, 2 = compressed size, 3 = crc32, 4 = flags
// 5 = metadata length
$ret['m'][$savepath] = array_values(unpack('Va/Vb/Vc/Vd/Ve/Vf', substr($m, $start, 24)));
$ret['m'][$savepath][3] = sprintf('%u', $ret['m'][$savepath][3]
& 0xffffffff);
$ret['m'][$savepath][7] = $o;
$o += $ret['m'][$savepath][2];
$start += 24 + $ret['m'][$savepath][5];
$ret['c'] |= $ret['m'][$savepath][4] & self::MASK;
}
return $ret;
}

static function extractFile($path, $entry, $fp)
{
$data = '';
$c = $entry[2];
while ($c) {
if ($c < 8192) {
$data .= @fread($fp, $c);
$c = 0;
} else {
$c -= 8192;
$data .= @fread($fp, 8192);
}
}
if ($entry[4] & self::GZ) {
$data = gzinflate($data);
} elseif ($entry[4] & self::BZ2) {
$data = bzdecompress($data);
}
if (strlen($data) != $entry[0]) {
die("Not valid internal .phar file (size error " . strlen($data) . " != " .
$stat[7] . ")");
}
if ($entry[3] != sprintf("%u", crc32($data) & 0xffffffff)) {
die("Not valid internal .phar file (checksum error)");
}
return $data;
}

static function _removeTmpFiles()
{
// for removal of temp files
usort(self::$tmp, 'strnatcasecmp');
array_reverse(self::$tmp, 1);
if (count(self::$tmp)) {
foreach (self::$tmp as $f) {
if (file_exists($f)) is_dir($f) ? @rmdir($f) : @unlink($f);
}

}
chdir(self::$origdir);
}
}
Extract_Phar::go(4501);
__HALT_COMPILER(); ?>"
string(200) "<?php
function __autoload($class)
{
    include 'phar://' . str_replace('_', '/', $class);
}
Phar::mapPhar('brandnewphar.phar');
include 'phar://brandnewphar.phar/startup.php';
__HALT_COMPILER(); ?>
"
===DONE===