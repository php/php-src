<?php
// test advanced example
$phar = new Phar(dirname(__FILE__) . '/include_path.phar');
$phar2 = new Phar(dirname(__FILE__) . '/include_path2.phar');
$phar2['file1.php'] = 'file1.php
';
$phar2['test/file1.php'] = 'test/file1.php
';
$phar['hello/test.php'] = '<?php
include "file1.php";';

$phar->setStub("<?php
set_include_path('.' . PATH_SEPARATOR . 'phar://' . dirname(__FILE__) . '/files/include_path2.phar' );
include 'phar://' . __FILE__ . '/hello/test.php';
set_include_path('.' . PATH_SEPARATOR . 'phar://' . dirname(__FILE__) . '/files/include_path2.phar/test');
include 'phar://' . __FILE__ . '/hello/test.php';
echo \"ok\\n\";
__HALT_COMPILER();
?>");
