--TEST--
phpinfo() mysqli section
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    include("connect.inc");

    @ob_clean();
    ob_start();
    phpinfo();
    $phpinfo = ob_get_contents();
    ob_end_clean();

    /* all versions should at least dump this minimum information */
    if (!stristr($phpinfo, "mysqli support"))
        printf("[001] ext/mysqli should have exposed itself.\n");

    if (!stristr($phpinfo, "client api library version"))
        printf("[002] ext/mysqli should have exposed the library version.\n");

    if (!stristr($phpinfo, "mysqli.default_host"))
        printf("[003] php.ini setting mysqli.default_host not shown.\n");

    if (!stristr($phpinfo, "mysqli.default_port"))
        printf("[004] php.ini setting mysqli.default_port not shown.\n");

    if (!stristr($phpinfo, "mysqli.default_pw"))
        printf("[005] php.ini setting mysqli.default_pw not shown.\n");

    if (!stristr($phpinfo, "mysqli.default_socket"))
        printf("[006] php.ini setting mysqli.default_socket not shown.\n");

    if (!stristr($phpinfo, "mysqli.default_user"))
        printf("[007] php.ini setting mysqli.default_user not shown.\n");

    if (!stristr($phpinfo, "mysqli.max_links"))
        printf("[008] php.ini setting mysqli.max_links not shown.\n");

    $expected = array(
        'size',
        'mysqli.allow_local_infile', 'mysqli.local_infile_directory',
        'mysqli.allow_persistent', 'mysqli.max_persistent'
    );
    foreach ($expected as $k => $entry)
        if (!stristr($phpinfo, $entry))
            printf("[010] Could not find entry for '%s'\n", $entry);

    print "done!";
?>
--EXPECT--
done!
