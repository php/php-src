--TEST--
Bug #61374: html_entity_decode tries to decode code points that don't exist in ISO-8859-1
--FILE--
<?php
echo html_entity_decode('&OElig;', 0, 'ISO-8859-1');
--EXPECT--
&OElig;
