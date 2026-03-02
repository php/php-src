--TEST--
Bug #63389 (Missing context check on libxml_set_streams_context() causes memleak)
--EXTENSIONS--
libxml
--FILE--
<?php
try {
    libxml_set_streams_context("a");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
$fp = fopen("php://input", "r");
try {
    libxml_set_streams_context($fp);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
echo "okey";
?>
--EXPECT--
libxml_set_streams_context(): Argument #1 ($context) must be of type resource, string given
libxml_set_streams_context(): supplied resource is not a valid Stream-Context resource
okey
