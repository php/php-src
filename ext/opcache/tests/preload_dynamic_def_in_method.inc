<?php
class Test {   
    function method() {      
        function dynamic() {   
            echo "dynamic";
        }
    }      
}       
        
$test = new Test; 
$test->method();
