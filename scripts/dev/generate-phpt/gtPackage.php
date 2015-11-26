<?php

/**
 * This creates a standalone phar file with all of the PHP source included. To run the
 * phar just type 'php generate-phpt.phar <options>' at the command line.
 */

if (Phar::canWrite()) {
  echo "Writing phar archive\n";
} else {
  echo "Unable to write archive, check that phar.readonly is 0 in your php.ini\n";
  exit();
}
$thisDir = dirname(__FILE__);
$pharPath = substr($thisDir, 0, -strlen('/generate-phpt'));

$phar = new Phar($pharPath.'/generate-phpt.phar');

$phar->buildFromDirectory($thisDir.'/src');

$stub = <<<ENDSTUB
<?php
Phar::mapPhar('generate-phpt.phar');
require 'phar://generate-phpt.phar/generate-phpt.php';
__HALT_COMPILER();
ENDSTUB;

$phar->setStub($stub);

?>
