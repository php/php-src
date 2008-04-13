--TEST--
Phar: phar:// opendir
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$files['a.php']   = '<?php echo "This is a\n"; require \''.$pname.'/b.php\'; ?>';      
$files['b.php']   = '<?php echo "This is b\n"; require \''.$pname.'/b/c.php\'; ?>';    
$files['b/c.php'] = '<?php echo "This is b/c\n"; require \''.$pname.'/b/d.php\'; ?>';  
$files['b/d.php'] = '<?php echo "This is b/d\n"; require \''.$pname.'/e.php\'; ?>';    
$files['e.php']   = '<?php echo "This is e\n"; ?>';                                  

include 'files/phar_test.inc';

function dump($phar, $base)
{
	var_dump(str_replace(dirname(__FILE__), '*', $phar) . $base);
	$dir = opendir($phar . $base);
	if ($base == '/')
	{
		$base = '';
	}
	while (false !== ($entry = readdir($dir))) {
		$entry = $base . '/' . $entry;
		var_dump($entry);
		var_dump(is_dir($phar . $entry));
		if (is_dir($phar . $entry))
		{
			dump($phar, $entry);
		}
	}
}

dump($pname, '/');

$a = opendir($pname);
// this may stop working in future versions, but is here for code coverage purposes
echo "fseek on dir handle\n";
var_dump(fseek($a, 0, SEEK_END), ftell($a));
var_dump(fseek($a, -1), ftell($a));
var_dump(fseek($a, 1), ftell($a));
echo "fwrite on dir handle\n";
var_dump(fwrite($a, 'hi'));
closedir($a);
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
string(%d) "phar://*/027.phar.php/"
string(6) "/a.php"
bool(false)
string(2) "/b"
bool(true)
string(%d) "phar://*/027.phar.php/b"
string(8) "/b/c.php"
bool(false)
string(8) "/b/d.php"
bool(false)
string(6) "/b.php"
bool(false)
string(6) "/e.php"
bool(false)
fseek on dir handle
int(0)
int(4)
int(-1)
int(4)
int(0)
int(1)
fwrite on dir handle
int(0)
===DONE===
