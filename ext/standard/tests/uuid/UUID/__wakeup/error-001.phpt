--TEST--
UUID::__wakeup throws UnexpectedValueException if binary property value is not of type string
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
    unserialize("O:4:\"UUID\":1:{s:12:\"\0UUID\0binary\";N;}");
}
catch (UnexpectedValueException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Expected value of type string, but found null
