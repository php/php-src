--TEST--
Bug #26862 (ob_flush() before output_reset_rewrite_vars() results in data loss)
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
html_errors=0
session.use_trans_sid=0
session.save_handler=files
session.trans_sid_tags="a=href,area=href,frame=src,form="
url_rewriter.tags="a=href,area=href,frame=src,form="
--FILE--
<?php
session_start();
output_add_rewrite_var('var', 'value');

echo '<a href="file.php">link</a>';

ob_flush();

output_reset_rewrite_vars();
echo '<a href="file.php">link</a>';
?>
--EXPECT--
<a href="file.php?var=value">link</a><a href="file.php">link</a>
