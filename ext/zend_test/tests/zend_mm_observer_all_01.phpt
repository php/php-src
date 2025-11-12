--TEST--
ZendMM Observer: Observe all
--EXTENSIONS--
zend_test
--INI--
zend_test.zend_mm_observer.enabled=1
--FILE--
<?php
echo "done.";
?>
--EXPECTREGEX--
.*ZendMM Observer enabled.*
(malloc|freed|realloc).*
(malloc|freed|realloc).*
(malloc|freed|realloc).*
(malloc|freed|realloc).*
(malloc|freed|realloc).*
(malloc|freed|realloc).*
(malloc|freed|realloc).*
.*shutdown in progress: full\(0\), silent\(1\).*
.*done\..*ZendMM Observer disabled.*
