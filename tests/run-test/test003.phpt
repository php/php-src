--TEST--
EXPECTREGEX
--FILE--
abcde12314235xyz34264768286abcde
--EXPECTREGEX--
[abcde]+[0-5]*xyz[2-8]+abcde