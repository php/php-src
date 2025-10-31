--TEST--
GH-20302 (Freeing a phar alias may invalidate PharFileInfo objects)
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__.'/gh20302.phar';
$pname = 'phar://' . $fname;
$file = "<?php
__HALT_COMPILER(); ?>";
$files = array();
$files['here'] = 'a';
include __DIR__.'/files/phar_test.inc';
$b = new PharFileInfo($pname . '/here');

// Create new phar with same alias and open it
@mkdir(__DIR__.'/gh20302');
$fname = __DIR__.'/gh20302/gh20302.phar';
$pname = 'phar://' . $fname;
include __DIR__.'/files/phar_test.inc';
try {
    new Phar($fname);
} catch (UnexpectedValueException $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__.'/gh20302/gh20302.phar');
@unlink(__DIR__.'/gh20302.phar');
@rmdir(__DIR__.'/gh20302');
?>
--EXPECTF--
Cannot open archive "%sgh20302.phar", alias is already in use by existing archive
