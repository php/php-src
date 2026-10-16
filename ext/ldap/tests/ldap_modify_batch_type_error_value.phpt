--TEST--
ldap_modify_batch(): the type errors name the type of the rejected value
--EXTENSIONS--
ldap
--FILE--
<?php
$l = ldap_connect('ldap://127.0.0.1:1');
foreach ([
    [["attrib" => "a", "modtype" => [], "values" => ["v"]]],
    [["attrib" => "a", "modtype" => "x", "values" => ["v"]]],
    [["attrib" => "a", "modtype" => LDAP_MODIFY_BATCH_ADD, "values" => "v"]],
    [["attrib" => "a", "modtype" => LDAP_MODIFY_BATCH_ADD, "values" => 5]],
] as $mods) {
    try {
        ldap_modify_batch($l, "cn=x", $mods);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
TypeError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "modtype" must be of type int, array given
TypeError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "modtype" must be of type int, string given
TypeError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "values" must be of type array, string given
TypeError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "values" must be of type array, int given
