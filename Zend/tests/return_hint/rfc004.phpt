--TEST--
Missing return type on override

--FILE--
<?php

class User {}

interface UserGateway {
    function find($id) : User; 
}

class UserGateway_MySql implements UserGateway {
    // must return User or subtype of User
    function find($id) {
        return new User;
    } 
}

--EXPECTF--
Fatal error: Declaration of UserGateway_MySql::find should be compatible with UserGateway::find($id) : User, return type missing in %s on line 9
