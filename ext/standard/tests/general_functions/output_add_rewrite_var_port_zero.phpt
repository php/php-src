--TEST--
output_add_rewrite_var() preserves an explicit port zero
--INI--
url_rewriter.tags="a=href,form=action"
url_rewriter.hosts="php.net"
--FILE--
<?php
ob_start();
output_add_rewrite_var('key', 'value');
?>
<a href="http://php.net/path">no port</a>
<a href="http://php.net:0/path">zero port</a>
<a href="http://php.net:8080/path">nonzero port</a>
<form action="http://php.net/path" method="post">no port</form>
<form action="http://php.net:0/path" method="post">zero port</form>
<form action="http://php.net:8080/path" method="post">nonzero port</form>
<?php
ob_end_flush();
?>
--EXPECT--
<a href="http://php.net/path?key=value">no port</a>
<a href="http://php.net:0/path?key=value">zero port</a>
<a href="http://php.net:8080/path?key=value">nonzero port</a>
<form action="http://php.net/path?key=value" method="post"><input type="hidden" name="key" value="value" />no port</form>
<form action="http://php.net:0/path?key=value" method="post"><input type="hidden" name="key" value="value" />zero port</form>
<form action="http://php.net:8080/path?key=value" method="post"><input type="hidden" name="key" value="value" />nonzero port</form>
