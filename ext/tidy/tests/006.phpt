--TEST--
Verbose tidy_get_error_buffer()
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 

    $tidy = tidy_create();
    
    tidy_parse_string($tidy, "<HTML><asd asdf></HTML>");
    
    echo tidy_get_error_buffer($tidy, true);

?>
--EXPECT--
line 1 column 1 - Warning: missing <!DOCTYPE> declaration
line 1 column 7 - Error: <asd> is not recognized!
line 1 column 7 - Warning: discarding unexpected <asd>
line 1 column 17 - Warning: discarding unexpected </html>
line 1 column 7 - Warning: inserting missing 'title' element