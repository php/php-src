--TEST--
phpinfo() with clickable anchor tags
--CGI--
--FILE--
<?php
phpinfo();
?>
--EXPECTF--
%a
<h2><a name="module_core" href="#module_core">Core</a></h2>
%a
