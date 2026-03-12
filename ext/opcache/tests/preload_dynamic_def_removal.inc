<?php
class Test {   
    function method() {      
        function dynamic() {   
            echo "dynamic\n";
        }
    }      

    public int $hook {
        get {
            function dynamic_in_hook() {
                echo "dynamic in hook\n";
            }
            return 1;
        }
    }
}       

function func() {
    function dynamic2() {
        echo "dynamic2\n";
    }
}
        
$test = new Test; 
$test->method();
func();
$test->hook;
