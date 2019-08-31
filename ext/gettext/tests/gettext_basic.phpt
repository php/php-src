--TEST--
Gettext basic test
--SKIPIF--
<?php
	if (!extension_loaded("gettext")) {
		die("skip\n");
	}
	if (!setlocale(LC_ALL, 'fi_FI')) {
		die("skip fi_FI locale not supported.");
	}
?>
--FILE--
<?php

chdir(__DIR__);
putenv('LC_ALL=fi_FI');
setlocale(LC_ALL, 'fi_FI');
bindtextdomain ("messages", "./locale");
textdomain ("messages");
echo gettext("Basic test"), "\n";
echo _("Basic test"), "\n";

?>
--EXPECT--
Perustesti
Perustesti
