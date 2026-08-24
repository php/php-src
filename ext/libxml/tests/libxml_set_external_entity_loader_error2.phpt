--TEST--
libxml_set_external_entity_loader() with non-callable argument
--EXTENSIONS--
libxml
--FILE--
<?php
try {
	libxml_set_external_entity_loader('nonexistent_function');
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: libxml_set_external_entity_loader(): Argument #1 ($resolver_function) must be a valid callback or null, function "nonexistent_function" not found or invalid function name
