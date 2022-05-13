<?php
class Test {   
    function method() {      
        function dynamic() {   
            echo "dynamic\n";
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
