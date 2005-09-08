--TEST--
catch shouldn't call __autoload
--FILE--
<?php
function __autoload($name) {
	echo("AUTOLOAD '$name'\n");
	eval("class $name {}");
}


try {
} catch (A $e) {
}

try {
  throw new Exception();
} catch (B $e) {
} catch (Exception $e) {
	echo "ok\n";
}
?>
--EXPECT--
ok
