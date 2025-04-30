--TEST--
XSLTProcessor::$maxTemplateDepth modification validation bypass
--EXTENSIONS--
xsl
--FILE--
<?php

$proc = new XSLTProcessor();
foreach ([1,-1] as $value) {
    echo "--- Set to $value ---\n";
    try {
        $proc->maxTemplateDepth = $value;
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

echo "--- Get reference ---\n";

try {
    $ref =& $proc->maxTemplateDepth;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Unset ---\n";

try {
    unset($proc->maxTemplateDepth);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Dump ---\n";

var_dump($proc);

?>
--EXPECT--
--- Set to 1 ---
--- Set to -1 ---
XSLTProcessor::$maxTemplateDepth must be greater than or equal to 0
--- Get reference ---
Indirect modification of XSLTProcessor::$maxTemplateDepth is not allowed
--- Unset ---
Cannot unset XSLTProcessor::$maxTemplateDepth
--- Dump ---
object(XSLTProcessor)#1 (4) {
  ["doXInclude"]=>
  bool(false)
  ["cloneDocument"]=>
  bool(false)
  ["maxTemplateDepth"]=>
  int(1)
  ["maxTemplateVars"]=>
  int(15000)
}
