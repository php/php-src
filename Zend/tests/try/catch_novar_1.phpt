--TEST--
catch without capturing a variable
--FILE--
<?php

try {
    throw new Exception();
} catch (Exception) {
    echo "Exception\n";
}

try {
    throw new Exception();
} catch (Exception) {
    echo "Exception\n";
} catch (Error) {
    echo "FAIL\n";
}

try {
    throw new Exception();
} catch (Exception|Error) {
    echo "Exception\n";
} catch (Throwable) {
    echo "FAIL\n";
}

?>
--EXPECT--
Exception
Exception
Exception
