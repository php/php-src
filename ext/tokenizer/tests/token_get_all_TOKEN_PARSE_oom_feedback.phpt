--TEST--
TOKEN_PARSE feedback after a token is dropped on OOM
--EXTENSIONS--
tokenizer
--ENV--
USE_ZEND_ALLOC=0
USE_TRACKED_ALLOC=1
--FILE--
<?php
$code = '<?php X::continue;';
ini_set('memory_limit', memory_get_usage() + 1024);
token_get_all($code, TOKEN_PARSE);
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
