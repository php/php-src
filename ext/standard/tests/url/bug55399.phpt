--TEST--
Bug #55399 (parse_url() incorrectly treats ':' as a valid path)
--FILE--
<?php

try {
	parse_url(":");
} catch(ValueError $e) {
	echo $e->getMessage();
}

?>
--EXPECT--
Invalid path (:)
