--TEST--
PDO::ATTR_SERVER_INFO
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
    require_once __DIR__ . '/inc/mysql_pdo_test.inc';
    $db = MySQLPDOTest::factory();

    assert(('' == $db->errorCode()) || ('00000' == $db->errorCode()));

    $info = $db->getAttribute(PDO::ATTR_SERVER_INFO);
    if ('' == $info)
        printf("[001] Server info must not be empty\n");

    // Read-only?
    if (false !== $db->setAttribute(PDO::ATTR_SERVER_INFO, 'new uptime: 0s'))
        printf("[002] Wonderful, I can change the client version!\n");

    $new_info = $db->getAttribute(PDO::ATTR_SERVER_INFO);
    if (soundex($new_info) != soundex($info))
        printf("[003] Did we change it from '%s' to '%s'?\n", $info, $info);

    // lets hope we always run this in the same second as we did run the server info request...
    if (!$stmt = $db->query("SHOW STATUS LIKE '%uptime%'"))
        printf("[004] Cannot run SHOW STATUS, [%s]\n", $db->errorCode());
    else {
        if (!$row = $stmt->fetch(PDO::FETCH_NUM))
            printf("[005] Unable to fetch uptime, [%s]\n", $db->errorCode());
        else
            $uptime = $row[1];
        $stmt->closeCursor();
    }

    if (!preg_match('/Uptime/i', $info))
        printf("[006] Can't find uptime in server info '%s'\n", $info);

    if (isset($uptime)) {
        if (!preg_match('/Uptime: (\d+)/i', $info, $matches) || $uptime - $matches[1] > 1) {
            printf("[007] SHOW STATUS and server info have reported a different uptime, please check. Server info: '%s', SHOW STATUS: '%s'\n", $info, $uptime);
        }
    }

    print "done!";
?>
--EXPECT--
done!
