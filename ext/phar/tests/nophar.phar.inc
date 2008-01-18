<?php
$fname = dirname(__FILE__) . '/nophar.phar';
@unlink($fname);
$p = new Phar($fname);
$p['index.php'] = '<?php include "b/c.php";' . "\n";
$p['b/c.php'] = '<?php echo "in b\n";$a = fopen("index.php", "r", true);echo stream_get_contents($a);fclose($a);include dirname(__FILE__) . "/../d";';
$p['d'] = "in d\n";
?>