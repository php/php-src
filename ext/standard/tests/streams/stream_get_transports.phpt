--TEST--
array stream_get_transports ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--FILE--
<?php
print((is_array(stream_get_transports())) ? ("yes") : ("Test 'array stream_get_transports ( void );' has failed"));
?>
--EXPECT--
yes
