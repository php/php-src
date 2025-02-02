--TEST--
Early returns
--FILE--
<?php
$tree = [];
while (1) {
    $tree = $tree['parent'] ?? break;
}
echo 'tree is null' . PHP_EOL;

function getTopParent(array $tree) {
    $parent = null;
    while (1) {
        $parent = $tree['parent'] ?? return $parent;
        $tree = $parent;
    }
    return null;
}
var_dump(getTopParent([
    'level' => 0,
    'parent' => [
        'level' => 1,
        'parent' => null,
    ]
]));

function test1($userOrNull, $subscriptionOrNull) {
    $user = $userOrNull ?? return 1;
    $subscription = $subscriptionOrNull ?? return 2;
    $lastCard = $subscription->invoices[0]->charges[0]->cardDetails->card ?? return 3;

    return 4;
}
echo test1(null,null);
echo test1(new stdClass(),null);
echo test1(new stdClass(),new stdClass());
?>
--EXPECT--
tree is null
array(2) {
  ["level"]=>
  int(1)
  ["parent"]=>
  NULL
}
123
