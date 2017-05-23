--TEST--
UUID::VERSION_* constant definition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

var_dump(
    UUID::VERSION_1_TIME_BASED,
    UUID::VERSION_2_DCE_SECURITY,
    UUID::VERSION_3_NAME_BASED_MD5,
    UUID::VERSION_4_RANDOM,
    UUID::VERSION_5_NAME_BASED_SHA1
);

?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
int(5)
