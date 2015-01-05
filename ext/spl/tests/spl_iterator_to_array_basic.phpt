--TEST--
SPL: iterator_to_array, Test function to convert iterator to array
--CREDITS--
Chris Scott chris.scott@nstein.com
#testfest London 2009-05-09
--FILE--
<?php

iterator_to_array();//requires iterator as arg

?>
--EXPECTF--
Warning: iterator_to_array() expects at least 1 parameter, %s
