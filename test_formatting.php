<?php

function   greet(  $name,   $age  )
{
    if($age>18){
        echo "Hello, ".$name."! You are ".$age." years old.";
    }else{
        echo "Hi, ".$name."!";
    }

    $items = array( "apple","banana",   "cherry" );
    foreach(  $items   as   $item  ){
        echo $item ;
    }

    $result=0;
    for($i=0;$i<10;$i++){
        $result+=$i;
    }

    return    $result;
}

class    UserService
{
    private $db;
    public function __construct(  $db  ){
        $this->db=$db;
    }

    public function   getUser(  $id  )
    {
        $query="SELECT * FROM users WHERE id=".$id;
        return $this->db->query(  $query  );
    }
}
