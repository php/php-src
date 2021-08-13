--TEST--
Bug #63389 (Missing context check on libxml_set_streams_context() causes memleak)
--EXTENSIONS--
libxml
--FILE--
<?php
$fp = fopen("php://input", "r");
libxml_set_streams_context($fp);
try {
    libxml_set_streams_context("a");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
echo "okey";
?>
--EXPECT--
libxml_set_streams_context(): Argument #1 ($context) must be of type resource, string given
okey
