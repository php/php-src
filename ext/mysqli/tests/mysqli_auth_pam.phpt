--TEST--
PAM auth plugin
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('connect.inc');

if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf("SKIP Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
        $host, $user, $db, $port, $socket));
}

if ($link->server_version < 50500)
    die(sprintf("SKIP Needs MySQL 5.5 or newer, found MySQL %s\n", $link->server_info));

if (!$res = $link->query("SHOW PLUGINS"))
    die(sprintf("SKIP [%d] %s\n", $link->errno, $link->error));

$have_pam = false;
while ($row = $res->fetch_assoc()) {
    if (isset($row['Name']) && in_array($row['Name'], array('pam', 'authentication_pam', 'auth_pam_compat'))) {
        $have_pam = $row['Name'];
        break;
    }
}
$res->close();

if (!$have_pam)
  die("SKIP Server PAM plugin not installed");

if ($have_pam == 'pam') {
    /* MariaDB - needs system variable pam_use_cleartext_plugin=ON to be set */
    if (!$res = mysqli_query($link, 'SHOW GLOBAL VARIABLES LIKE "pam_use_cleartext_plugin"'))
        die(sprintf("SKIP MariaDB probe of GLOBAL VARIABLES failed [%d] %s\n",
                    mysqli_errno($link), mysqli_error($link)));
    $pam_use_cleartext_plugin = mysqli_fetch_row($res);
    mysqli_free_result($res);
    if (!$pam_use_cleartext_plugin or $pam_use_cleartext_plugin[1]!='ON')
        die("SKIP Server setting pam_use_cleartext_plugin!=ON");

    $pam_service = file_get_contents('/etc/pam.d/mysql');
} elseif ($have_pam == 'authentication_pam') {
    /*
       required MySQL syntax:
       https://dev.mysql.com/doc/refman/8.0/en/pam-pluggable-authentication.html#pam-pluggable-authentication-usage
    */
    $have_pam .= " AS 'mysql-unix'";
    $pam_service = file_get_contents('/etc/pam.d/mysql-unix');
} else {
    $pam_service = file_get_contents('/etc/pam.d/mysql');
}
$auth = 0;
$account = 0;
foreach (explode("\n", $pam_service) as $line)
{
    if (preg_match('/^auth/', $line)) {
        $auth = 1;
    } elseif (preg_match('/^account/', $line)) {
        $account = 1;
    }
}
if (!$auth) {
    die("SKIP pam service file missing 'auth' directive");
}
if (!$account) {
    die("SKIP pam service file missing 'account' directive");
}

if (!posix_getpwnam('pamtest')) {
    die("SKIP no pamtest user");
}
/* Password of user 'pamtest' should be set to 'pamtest' */

mysqli_query($link, 'DROP USER pamtest');
mysqli_query($link, 'DROP USER pamtest@localhost');

if (!mysqli_query($link, "CREATE USER pamtest@'%' IDENTIFIED WITH $have_pam") ||
    !mysqli_query($link, "CREATE USER pamtest@'localhost' IDENTIFIED WITH $have_pam")) {
    printf("skip Cannot create second DB user [%d] %s", mysqli_errno($link), mysqli_error($link));
    mysqli_close($link);
    die("skip CREATE USER failed");
}

if (!$link->query("CREATE TABLE test (id INT)") || !$link->query("INSERT INTO test(id) VALUES (1)"))
    die(sprintf("SKIP [%d] %s\n", $link->errno, $link->error));



if (!mysqli_query($link, sprintf("GRANT SELECT ON TABLE %s.test TO pamtest@'%%'", $db)) ||
    !mysqli_query($link, sprintf("GRANT SELECT ON TABLE %s.test TO pamtest@'localhost'", $db))) {
    printf("skip Cannot grant SELECT to user [%d] %s", mysqli_errno($link), mysqli_error($link));
    mysqli_close($link);
    die("skip GRANT failed");
}
?>
--INI--
max_execution_time=240
--FILE--
<?php
    require_once('connect.inc');
    require_once('table.inc');

    if (!$link = my_mysqli_connect($host, 'pamtest', 'pamtest', $db, $port, $socket)) {
        printf("[001] Cannot connect to the server using host=%s, user=pamtest, passwd=pamtest dbname=%s, port=%s, socket=%s\n",
            $host, $db, $port, $socket);
    } else {

      if (!$res = $link->query("SELECT id FROM test WHERE id = 1"))
          printf("[002] [%d] %s\n", $link->errno, $link->error);

      if (!$row = mysqli_fetch_assoc($res)) {
          printf("[003] [%d] %s\n", $link->errno, $link->error);
      }

      if ($row['id'] != 1) {
          printf("[004] Expecting 1 got %s/'%s'", gettype($row['id']), $row['id']);
      }

      $res->close();
      $link->close();
    }

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
    mysqli_query($link, 'DROP USER pamtest');
    mysqli_query($link, 'DROP USER pamtest@localhost');
?>
--EXPECTF--
done!
