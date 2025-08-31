--TEST--
Bug #79987 (Memory leak in SplFileInfo because of missing zend_restore_error_handling())
--FILE--
<?php
class BadSplFileInfo extends SplFileInfo {
    public function __construct() {
    }
}
$x = new BadSplFileInfo();
set_error_handler(function ($type, $msg, $file, $line, $context = []) {
    echo "ops\n";
});
try {
    var_dump($x->getLinkTarget());
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($x->getFilename());
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($x->getExtension());
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump($x->getBasename());
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Object not initialized
Object not initialized
Object not initialized
Object not initialized
