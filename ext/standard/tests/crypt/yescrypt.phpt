--TEST--
Test crypt() with yescrypt
--FILE--
<?php

var_dump(crypt("test", '$y$'));
var_dump(crypt("test", '$y$$'));
var_dump(crypt("test", '$y$j$'));
var_dump(crypt("test", '$y$j9$'));
var_dump(crypt("test\0x", '$y$j9T$salt'.chr(0)));
var_dump(crypt("test\0", '$y$j9T$salt'));
var_dump(crypt("test\0x", '$y$j9T$salt'));
var_dump(crypt("\0", '$y$j9T$salt'));

var_dump(crypt("test", '$y$j9T$'));
var_dump(crypt("test", '$y$j9T$salt'));
var_dump(crypt("test", '$y$j9T$salt$'));
var_dump(crypt("", '$y$j9T$salt'));

var_dump(crypt("", '$7$400.../....$'));
var_dump(crypt("", '$7$400.../....$salt$'));

?>
--EXPECT--
string(2) "*0"
string(2) "*0"
string(2) "*0"
string(2) "*0"
string(2) "*0"
string(2) "*0"
string(2) "*0"
string(2) "*0"
string(51) "$y$j9T$$6tN6tt5mmPHxQskcf5Oi7Sb.1nKYbi5cOZgTiMq7Qw4"
string(55) "$y$j9T$salt$a9CZafQyDF042zUCgPAhoF7Zd5phBweZqIIw6SMCTh."
string(55) "$y$j9T$salt$a9CZafQyDF042zUCgPAhoF7Zd5phBweZqIIw6SMCTh."
string(55) "$y$j9T$salt$sE5vvd.NbRw0CRzUgcEQ/PZMH4hmete7N5s3qN09F12"
string(58) "$7$400.../....$fsLd.toTUvgzSAYmoHbKwQGAmqLK6y.yIpW2WKuemOA"
string(63) "$7$400.../....$salt$3SJITk6BqtXkmuOQkPe7e.yClr8MVXc6twSB2ZBHPE3"
