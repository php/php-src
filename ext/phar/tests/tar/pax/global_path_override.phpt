--TEST--
Global path override with PAX header
--EXTENSIONS--
phar
--FILE--
<?php

try {
    new PharData(__DIR__ . '/../files/gh19311/global_path_override.tar');
} catch (UnexpectedValueException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
phar error: "%s" is a corrupted tar file (invalid global pax header: unsupported global path override)
