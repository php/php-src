--TEST--
catch shouldn't call autoloader
--FILE--
<?php

spl_autoload_register(function ($name) {
    echo("AUTOLOAD '$name'\n");
    eval("class $name {}");
});

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
