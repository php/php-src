--TEST--
tidy_diagnose()
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
    tidy_parse_string("<HTML></HTML>");
    tidy_diagnose();
    echo tidy_get_error_buffer();

?>
--EXPECT--

line 1 column 1 - Warning: missing <!DOCTYPE> declaration
line 1 column 7 - Warning: discarding unexpected </html>
line 1 column 14 - Warning: inserting missing 'title' element
Info: Document content looks like HTML 3.2
3 warnings, 0 errors were found!