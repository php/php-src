--TEST--
PDO::ATTR_SERVER_VERSION
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
    $db = MySQLPDOTest::factory();

    assert(('' == $db->errorCode()) || ('00000' == $db->errorCode()));

    $version = $db->getAttribute(PDO::ATTR_SERVER_VERSION);
    if ('' == $version)
        printf("[001] Server version must not be empty\n");

    // Ideally the server version would be an integer - as documented but BC break!
    // If its a version string it should be of the format \d+\.\d+\.\d+.*

    if (is_string($version)) {
        // Its not an int like documented but a string - maybe for BC reasons...
        if (!preg_match('/(\d+)\.(\d+)\.(\d+)(.*)/', $version, $matches))
            printf("[002] Client version string seems wrong, got '%s'\n", $version);
        else {
            // Difficult to define any meaningful constraints
            // A possible better check would be calling mysqli_get_server_version() and
            // comparing what we get. However, mysqli_get_server_version() needs a mysqli handle
            // for which in turn one needs to parse the PDO test environment variables
            // for connection parameter...
            if ($matches[1] < 3)
                printf("[003] Strange major version: '%s'. Should be more than 3\n", $matches[1]);
            if ($matches[2] < 0)
                printf("[004] Minor version should be at least 0, got '%s'\n", $matches[2]);
            if ($matches[3] < 0)
                printf("[005] Sub version should be at least 0, got '%s'\n", $matches[2]);
        }
    } else if (is_int($version)) {
        // Lets accept also int if it follows the rules from the original MYSQL C API
        $major = floor($version / 10000);
        $minor = floor(($version - ($main * 10000)) / 100);
        $sub = $version - ($main * 10000) - ($minor * 100);
        if ($major < 3)
            printf("[006] Strange major version: '%s'. Should be more than 3\n", $major);
        if ($minor < 0)
            printf("[007] Minor version should be at least 0, got '%s'\n", $minor);
        if ($sub < 0)
            printf("[008] Sub version should be at least 0, got '%s'\n", $sub);
    }

    // Read-only?
    if (false !== $db->setAttribute(PDO::ATTR_CLIENT_VERSION, '1.0'))
        printf("[009] Wonderful, I can change the client version!\n");

    $new_version = $db->getAttribute(PDO::ATTR_SERVER_VERSION);
    if ($new_version !== $version)
        printf("[010] Did we change it from '%s' to '%s'?\n", $version, $new_version);

    print "done!";
?>
--EXPECT--
done!
