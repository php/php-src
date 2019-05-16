--TEST--
Bug #75236: infinite loop when printing an error-message
--FILE--
<?php

    ini_set('html_errors', true);
    ini_set('default_charset', 'ISO-8859-2');

    printf ("before getfilecontent\n");
    file_get_contents ('no/suchfile');
    printf ("after getfilecontent\n");

?>
--EXPECTF--
before getfilecontent
<br />
<b>Warning</b>:  file_get_contents(no/suchfile): failed to open stream: No such file or directory in <b>%s</b> on line <b>7</b><br />
after getfilecontent
