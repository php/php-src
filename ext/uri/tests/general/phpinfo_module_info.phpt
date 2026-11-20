--TEST--
Test phpinfo() - URI module information
--FILE--
<?php

ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();

var_dump(str_contains($info, "URI support => active"));
var_dump(
    str_contains($info, "uriparser bundled version") ||
    (
        str_contains($info, "uriparser compiled version")
        &&
        str_contains($info, "uriparser loaded version")
    )
);

?>
--EXPECT--
bool(true)
bool(true)
