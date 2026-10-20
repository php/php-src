--TEST--
ldap_set_rebind_proc() with a trampoline
--EXTENSIONS--
ldap
--SKIPIF--
<?php
if (!function_exists('ldap_set_rebind_proc')) die("skip ldap_set_rebind_proc() not available");
require_once('skipifbindfailure.inc');
?>
--FILE--
<?php
/*** NOTE: THE TRAMPOLINE IS NOT CALLED AS WE DON'T TEST THE REBINDING HAPPENS AS WE NEED MULTIPLE LDAP SERVERS ***/

require "connect.inc";

class TrampolineTest {
    public function __construct(private $user, private $password, private $protocol_version) {}
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        var_dump(count($arguments));
        if ($name === 'trampolineThrow') {
            throw new Exception('boo');
        }
        if ($name === 'trampolineWrongType') {
            return ['not an int'];
        }
        // required by most modern LDAP servers, use LDAPv3
        ldap_set_option($ldap, LDAP_OPT_PROTOCOL_VERSION, $this->$protocol_version);
        if (!ldap_bind($ldap, $this->$user, $this->$password)) {
            // Failure
            print "Cannot bind";
            return 1;
        }
        // Success
        return 0;
    }
}
$o = new TrampolineTest($user, $passwd, $protocol_version);
$callback = [$o, 'trampoline'];
$callbackThrow = [$o, 'trampolineThrow'];
$callbackWrongType = [$o, 'trampolineWrongType'];

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
var_dump(ldap_set_rebind_proc($link, $callback));
var_dump(ldap_set_rebind_proc($link, null));
var_dump(ldap_set_rebind_proc($link, $callbackThrow));
var_dump(ldap_set_rebind_proc($link, null));
var_dump(ldap_set_rebind_proc($link, $callbackWrongType));

var_dump(ldap_unbind($link));
try {
    var_dump(ldap_set_rebind_proc($link, $callback));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Error: LDAP connection has already been closed
