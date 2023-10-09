<?php
require_once 'connect.inc';

function skipifunsupportedcontrol($oid)
{
    global $uri, $user, $passwd, $protocol_version, $base;
    $link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
    $result = ldap_read($link, '', '(objectClass=*)', ['supportedControl']);
    if (!in_array($oid, ldap_get_entries($link, $result)[0]['supportedcontrol'])) {
        die(sprintf("skip Unsupported control %s", $oid));
    }
}
?>
