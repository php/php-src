--TEST--
environ() basic tests
--INI--
variables_order="GPCS"
--ENV--
return <<<END
FOO="bar"
END;
--FILE--
<?php

var_dump(environ()["FOO"]);

putenv("FOO=baz");

var_dump(environ()["FOO"]);

echo "Done\n";
?>
--EXPECTF--
string(5) ""bar""
string(3) "baz"
Done
