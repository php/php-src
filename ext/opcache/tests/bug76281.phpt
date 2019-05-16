--TEST--
Bug #76281: Opcache causes incorrect "undefined variable" errors
--FILE--
<?php

function test($r, $action) {
    $user_sub_resource = in_array($action, array('get_securityquestions', 'get_status', 'get_groupstats'));

    $user_id = null;
    if ($user_sub_resource && isset($r['user_id'])) {
        $user_id = $r['user_id'];
    }
    else if (isset($r['id']))  {
        $user_id = $r['id'];
    }

    if ($user_sub_resource) {
        return 'foo';
    }

    return 'bar';
}

var_dump(test(['user_id' => 1, 'id' => 2], 'foo'));

?>
--EXPECT--
string(3) "bar"
