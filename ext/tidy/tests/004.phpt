--TEST--
tidy_diagnose()
--SKIPIF--
<?php if (!extension_loaded("Tidy")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 

    $tidy = tidy_create();
    
    tidy_parse_string($tidy, "<HTML></HTML>");
    tidy_diagnose($tidy);
    echo tidy_get_error_buffer($tidy);

?>
--EXPECT--

line 1 column 1 - Warning: missing <!DOCTYPE> declaration
line 1 column 7 - Warning: discarding unexpected </html>
line 1 column 14 - Warning: inserting missing 'title' element
Info: Document content looks like HTML 3.2
3 warnings, 0 errors were found!