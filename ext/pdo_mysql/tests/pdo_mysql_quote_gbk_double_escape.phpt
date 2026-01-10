--TEST--
PDO_MYSQL: Test quoting of multibyte sequence with GBK vs utf8mb4
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

    $link = MySQLPDOTest::factory('PDO', ['charset' => 'GBK']);
    $quoted = $link->quote("\xbf\x27");
    $quoted_without_outer_quotes = substr($quoted, 1, -1);

    /* This should result in 5C BF 5C 27 for GBK instead of BF 5C 27 like with UTF8MB4.
     * To explain why the extra escaping takes place, let's assume we don't do that and see what happens.
     *
     * 1. First iteration, i.e. *from == 0xBF. This isn't a valid GBK multibyte sequence start,
     *    so the mb validity check fails.
     *    Without the character length check, we'd check if we need to escape the current character 0xBF.
     *    The character 0xBF isn't handled in the switch case so we don't escape it and append 0xBF to the output buffer.
     * 2. Second iteration, i.e. *from == 0x27. This isn't a valid start either, so we go to the escape logic.
     *     Note that 0x27 is the character ', so we have to escape! We write two bytes to the output:
     *     \ (this is 0x5C) and ' (this is 0x27).
     * 3. The function finished, let's look at the output: 0xBF 0x5C 0x27.
     *    Now we actually made a problem: 0xBF 0x5C is a valid GBK multibyte sequence!
     *    So we transformed an invalid multibyte sequences into a valid one, potentially corrupting data.
     *    The solution is to check whether it could have been part of a multibyte sequence, but the checks are less strict. */
    var_dump(bin2hex($quoted_without_outer_quotes));

    unset($link);

    // Compare with utf8mb4
    $link = MySQLPDOTest::factory('PDO', ['charset' => 'utf8mb4']);
    $quoted = $link->quote("\xbf\x27");
    $quoted_without_outer_quotes = substr($quoted, 1, -1);
    var_dump(bin2hex($quoted_without_outer_quotes));
?>
--EXPECT--
string(8) "5cbf5c27"
string(6) "bf5c27"
