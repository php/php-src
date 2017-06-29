--TEST--
UUID::__wakeup throws UnexpectedValueException if property value is not of type string
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

try {
    unserialize("O:12:\"PHP\Std\UUID\":1:{s:19:\"\0PHP\Std\UUID\0bytes\";N;}");
}
catch (UnexpectedValueException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Expected PHP\Std\UUID::$bytes value to be of type string, but found null
