--TEST--
GH-19892: Successful \Uri\WhatWg\Url::parse() with empty errors array
--FILE--
<?php

\Uri\WhatWg\Url::parse('https://example.com', errors: $errors);
var_dump($errors);

?>
--EXPECT--
array(0) {
}
