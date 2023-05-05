--TEST--
GH-9775: Pure enum in array_unique()
--FILE--
<?php

enum Test
{
    case AUTHENTICATED;
    case COURSES_ADMIN;
    case BUNDLES_ADMIN;
    case COURSES_REPORTING_ACCESS;
    case B2B_DASHBOARD_ACCESS;
    case INSTRUCTORS_ADMIN;
    case USERS_ADMIN;
    case COUPONS_ADMIN;
}

$instructorsAdmin = Test::INSTRUCTORS_ADMIN;

$data = [
    Test::COURSES_ADMIN,
    Test::COURSES_REPORTING_ACCESS,
    Test::BUNDLES_ADMIN,
    Test::USERS_ADMIN,
    Test::B2B_DASHBOARD_ACCESS,
    Test::B2B_DASHBOARD_ACCESS,
    Test::INSTRUCTORS_ADMIN,
    &$instructorsAdmin,
    Test::COUPONS_ADMIN,
    Test::AUTHENTICATED,
];

$data = array_unique($data, flags: SORT_REGULAR);

var_dump($data);

?>
--EXPECT--
array(8) {
  [0]=>
  enum(Test::COURSES_ADMIN)
  [1]=>
  enum(Test::COURSES_REPORTING_ACCESS)
  [2]=>
  enum(Test::BUNDLES_ADMIN)
  [3]=>
  enum(Test::USERS_ADMIN)
  [4]=>
  enum(Test::B2B_DASHBOARD_ACCESS)
  [6]=>
  enum(Test::INSTRUCTORS_ADMIN)
  [8]=>
  enum(Test::COUPONS_ADMIN)
  [9]=>
  enum(Test::AUTHENTICATED)
}
