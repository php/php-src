--TEST--
GH-15821 (Core dumped in Zend/Optimizer/zend_inference.c:4062)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1203
opcache.jit_buffer_size=64M
--FILE--
<?php
$allInternalFunctions = array();
$definedFunctions = get_defined_functions();
$internalFunctions = $definedFunctions['internal'];
$allInternalFunctions = array_merge($allInternalFunctions, $internalFunctions);
$allInternalFunctions = array_filter($allInternalFunctions, function($func) {
    strpos($func, 'posix_') !== 0;
});
echo "Done\n";
?>
--EXPECT--
Done
