--TEST--
Bug #32322 (Return values by reference broken( using self::),example singleton instance)
--INI--
error_reporting=4095
--FILE--
<?php
class test
{
    private static $instance = null;
    private $myname = '';
    
    private function __construct( $value = '' ) 
    {
        echo "New class $value created \n";
        $this -> myname = $value;
    }
    private function __clone() {}
    static public function getInstance()
    {
        if ( self::$instance == null )
        {
            self::$instance = new test('Singleton1');
        }
        else {
            echo "Using old class " . self::$instance -> myname . "\n";
        }
        return self::$instance;
    }
    static public function getInstance2()
    {
        static $instance2 = null;
        if ( $instance2 == null )
        {
            $instance2 = new test('Singleton2');
        }
        else {
            echo "Using old class " . $instance2 -> myname . "\n";
        }
        return $instance2;
    }
    public function __destruct() 
    {
        if ( defined('SCRIPT_END') )
        {
            echo "Class " . $this -> myname . " destroyed at script end\n";
        } else {
            echo "Class " . $this -> myname . " destroyed beforce script end\n";
        }
    }
}    
echo "Try static instance inside class :\n";
$getCopyofSingleton    = test::getInstance();
$getCopyofSingleton    = null;
$getCopyofSingleton    = &test::getInstance();
$getCopyofSingleton    = null;
$getCopyofSingleton    = test::getInstance();
echo "Try static instance inside function :\n";
$getCopyofSingleton2   = test::getInstance2();
$getCopyofSingleton2   = null;
$getCopyofSingleton2   = &test::getInstance2();
$getCopyofSingleton2   = null;
$getCopyofSingleton2   = test::getInstance2();

define('SCRIPT_END',1);
?>
--EXPECTF--
Try static instance inside class :
New class Singleton1 created 
Using old class Singleton1

Strict Standards: Only variables should be assigned by reference in %sbug32322.php on line 49
Using old class Singleton1
Try static instance inside function :
New class Singleton2 created 
Using old class Singleton2

Strict Standards: Only variables should be assigned by reference in %sbug32322.php on line 55
Using old class Singleton2
Class Singleton1 destroyed at script end
Class Singleton2 destroyed at script end
