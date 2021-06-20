--TEST--
Bug #74432, BC issue on undocumented connect string
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    /* This behavior is undocumented, but might be in use. Until there's no officially
        supported alternative, ensure changes doesn't cause BC breach. Otherwise,
        the test should be removed once the undocumented behavior changes. */

    require_once("connect.inc");

    $handle = mysqli_connect("$host:$port", $user, $passwd);

    var_dump($handle);

    if ($handle) {
        mysqli_close($handle);
    }
?>
--EXPECTF--
object(mysqli)#%d (%d) {
%A
}
