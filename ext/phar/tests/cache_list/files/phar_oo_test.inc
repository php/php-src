<?php

ini_set('date.timezone', 'GMT');

$fname = dirname(__FILE__) . '/phar_oo_test.phar.php';
$pname = 'phar://' . $fname;
$file = '<?php include "' . $pname . '/a.php"; __HALT_COMPILER(); ?>';

$files = array();

if (!isset($pharconfig)) $pharconfig = 0;

switch($pharconfig)
{
	default:
	case 0:
		$files['a.php']   = '<?php echo "This is a.php\n"; ?>';
		$files['b.php']   = '<?php echo "This is b.php\n"; ?>';
		$files['b/c.php'] = '<?php echo "This is b/c.php\n"; ?>';
		$files['b/d.php'] = '<?php echo "This is b/d.php\n"; ?>';
		$files['e.php']   = '<?php echo "This is e.php\n"; ?>';
		break;
	case 1:
		$files['a.csv'] =<<<EOF
1,2,3
2,a,b
3,"c","'e'"
EOF;
		break;
	case 2:
		$files['a.csv'] =<<<EOF
1,2,3
2,a,b
3,"c","'e'"
4
5,5

7,777
EOF;
		break;
	case 3:
		$files['a.php'] = '<?php echo new new class;';
		break;
}

$ftime = mktime(12, 0, 0, 3, 1, 2006);
include 'phar_test.inc';

?>
