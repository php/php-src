--TEST--
Bug #46900 (Unexpected behaviour in HEAD when output buffer callback returns null)
--FILE--
<?php
function return_null($string) {
	return null;
}

ob_start('return_null');
echo "You shouldn't see this.\n";
ob_end_flush();

echo 'done';
?>
--EXPECTF--
done