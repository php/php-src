--TEST--
SPL SplTempFileObject constructor sets correct defaults when pass 0 arguments
--FILE--
<?php
try {
    new SplTempFileObject('invalid');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
SplTempFileObject::__construct() expects parameter 1 to be integer (or convertible float, convertible string or boolean), string given
