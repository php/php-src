--TEST--
RFC example: missing return type on override
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
?>
--EXPECTF--
Fatal error: Declaration of UserGateway_MySql::find($id) must be compatible with UserGateway::find($id): User in %s on line 11
