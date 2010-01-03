--TEST--
Bug #50636 (MySQLi_Result sets values before calling constructor)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include ("connect.inc");

    class Book {
        private $title = 0;

        function __construct() {
            $this->title = 'foobar';
        }

        function __set($name, $value) {
            $this->{$name}   = $value;
        }
    }

    $link = new mysqli($host, $user, $passwd);
    var_dump($link->query('SELECT "PHP" AS title, "Rasmus" AS author')->fetch_object('Book'));
    echo "done!";
?>
--EXPECTF--
object(Book)#%d (2) {
  ["title:private"]=>
  string(3) "PHP"
  ["author"]=>
  string(6) "Rasmus"
}
done!
