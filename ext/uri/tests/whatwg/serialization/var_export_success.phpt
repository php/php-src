--TEST--
Test var_export() with Uri\WhatWg\Url
--FILE--
<?php

var_export(new Uri\WhatWg\Url("https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists"));
echo "\n";

?>
--EXPECT--
\Uri\WhatWg\Url::__set_state(array(
))
