--TEST--
GH-10765: Throw on negative cookie expiration timestamp
--INI--
date.timezone=UTC
--FILE--
<?php

try {
    setcookie("name", "value", ['expires' => -1]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
setcookie(): "expires" option cannot be negative
--EXPECTHEADERS--
