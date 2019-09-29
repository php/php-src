--TEST--
Bug #60677: CGI doesn't properly validate shebang line contains #!
--CGI--
--FILE--
#<?php echo "Hello World\n"; ?>
Second line.
--EXPECT--
#Hello World
Second line.
